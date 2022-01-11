/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2021 IRCReborn Devs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <common/util.h>
#include <ui/widget.h>
#include <ui/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/attrib.h>
#include <common/logger.h>
#ifdef WIN32
#include <windows.h> // ugh.
#else
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <sys/time.h>
#include <ui/util/font_search.h>
#endif

window_t** windows      = 0;
int        window_count = 0;
int        window_count_2 = 0;

void __DEFAULT_window_handle_bg_tasks(window_t* a) {};
void __DEFAULT_window_handle_resize(window_t* a) {};

#ifdef WIN32
window_t* resolve_window(HWND a) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i]->window == a) return windows[i];
    }
    return (window_t*)-1;
}
#endif

// this is bad. really bad.
widget_t** window_sort_z(window_t* window) {
    widget_t** sorted   = (widget_t**)malloc(window->widget_count * sizeof(void*));
    widget_t** unsorted = (widget_t**)malloc(window->widget_count * sizeof(void*));
    int        ulen     = window->widget_count;
    int        slen     = 0;

    memcpy(unsorted, window->widgets, window->widget_count * sizeof(void*));

    while (ulen > 0) {
        widget_t*  hi = unsorted[0];
        widget_t** nunsorted = (widget_t**)malloc(window->widget_count * sizeof(void*));
        int        nlen = 0;

        for (int i = 0; i < ulen; i++) {        
            if (hi->z < unsorted[i]->z) {
                hi = unsorted[i];
            }
        }

        sorted[slen] = hi;
        
        for (int i = 0; i < ulen; i++) {
            if (unsorted[i] != hi) {
                nunsorted[nlen] = unsorted[i];
                nlen++;
            }
        }

        ulen--;
        slen++;

        free(unsorted);
        unsorted = nunsorted;
    }

    free(unsorted);

    return sorted;
}

void window_close(window_t* window) {
    window->should_exit = 1;
}

void window_paint(window_t* window) {
#ifdef WIN32
    PAINTSTRUCT* ps = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
    RECT* rect = (RECT*)malloc(sizeof(RECT));
    SetRect(
        rect,
        0, 0,
        window->width, window->height
    );
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, ps);
    SelectObject(ps->hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(ps->hdc, W32RGBAC(window->bg_color));
    Rectangle(ps->hdc, rect->left, rect->top, rect->right, rect->bottom);
    EndPaint(window->window, ps);
    free(rect);
    free(ps);
#else
    xcb_alloc_color_reply_t* bg = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            window->bg_color.r << 8,
            window->bg_color.g << 8,
            window->bg_color.b << 8
        ),
        NULL
    );
    uint32_t maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
    uint32_t maskv[] = {
        bg->pixel,
        bg->pixel
    };
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    xcb_rectangle_t* rect = (xcb_rectangle_t*)malloc(sizeof(xcb_rectangle_t));
    rect->x = 0;
    rect->y = 0;
    rect->height = window->height;
    rect->width = window->width;
    xcb_poly_fill_rectangle(
        window->connection,
        window->window,
        window->gc,
        1, rect
    );
    free(rect);
#endif
    for (int i = 0; i < window->widget_count; i++) {
        widget_t* widget = window->widgets[i];
        widget->draw(widget, window);
    }
}

void window_left_mouse_down(window_t* window, int x, int y) {        
    window->mouse_left_down = 1; // the mouse button is down :catnod:
    for (int i = 0; i < window->widget_count; i++) {
        widget_t* widget = window->widgets[i];
        if (widget->x < x && x < widget->x + widget->width && widget->y < y && y < widget->y + widget->height) {
            widget->mousedown(widget, window, x, y);
        }
    }
}

void window_left_mouse_up(window_t* window, int x, int y) {
    window->mouse_left_down = 0;

    int clickdone = 0;
    int updone    = 0;

    widget_t** widgets = window_sort_z(window);
    int        widget_count = window->widget_count;

    // this is a click B)
    for (int i = 0; i < widget_count; i++) {
        widget_t* widget = widgets[i];
        if (widget->x < x && x < widget->x + widget->width && widget->y < y && y < widget->y + widget->height) {
            if (!updone) {
                updone = widget->mouseup(widget, window, x, y);
            }
            if (!clickdone) {
                clickdone = widget->clicked(widget, window, x, y);
            }
        }
    }

    free(widgets);
}

void window_scroll_up(window_t* window, int x, int y) {
    int done = 0;

    widget_t** widgets = window_sort_z(window);
    int        widget_count = window->widget_count;

    for (int i = 0; i < widget_count; i++) {
        widget_t* widget = widgets[i];
        if (widget->x < x && x < widget->x + widget->width && widget->y < y && y < widget->y + widget->height) {
            if (!done) {
                widget->scroll_up(widget, window);
            }
        }
    }

    free(widgets);
}

void window_scroll_down(window_t* window, int x, int y) {
    int done = 0;

    widget_t** widgets = window_sort_z(window);
    int        widget_count = window->widget_count;

    for (int i = 0; i < widget_count; i++) {
        widget_t* widget = widgets[i];
        if (widget->x < x && x < widget->x + widget->width && widget->y < y && y < widget->y + widget->height) {
            if (!done) {
                widget->scroll_down(widget, window);
            }
        }
    }

    free(widgets);
}

void window_mouse_move(window_t* window, int x, int y) {
    for (int i = 0; i < window->widget_count; i++) {
        widget_t* widget = window->widgets[i];
        if (widget->x < x && x < widget->x + widget->width && widget->y < y && y < widget->y + widget->height) {
            if (widget->hovered == 0) {
                widget->hovered = 1;
                widget->mousein(widget, window);
            }
            widget->mousemove(widget, window, x, y);
        } else {
            if (widget->hovered == 1) {
                widget->hovered = 0;
                widget->mouseout(widget, window);
            }
        }
    }    
}

void window_resized(window_t* window, int w, int h) {
    window->width = w;
    window->height = h;

    window->resized(window);
}

void window_keypress(window_t* window, uint32_t key, uint16_t mod) {
    widget_t* focused = window->focused;
            
    if (focused == 0) return;

    focused->keypress(focused, window, key, mod);
}

void window_run_bg_tasks(window_t* window) {
    window->handle_bg_tasks(window);
}

#ifdef WIN32
LRESULT window_proc(HWND window, UINT message, WPARAM thing, LPARAM otherthing) {
    window_t* me = resolve_window(window);
    switch (message) {
        // please also update the delete code in the xorg part
        case WM_CLOSE: {
            window_close(me);
            return 0;
        }
        case WM_PAINT: {
            window_paint(me);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            ClipCursor(&me->client_rect);
            int cx = LOWORD(otherthing); 
            int cy = HIWORD(otherthing);
            window_left_mouse_down(me, cx, cy);
            return 0;
        }
        case WM_LBUTTONUP: {
            int cx = LOWORD(otherthing); 
            int cy = HIWORD(otherthing);
            ClipCursor(0);
            window_left_mouse_up(me, cx, cy);
            return 0;
        }
        case WM_MOUSEMOVE: {
            int cx      = LOWORD(otherthing); 
            int cy      = HIWORD(otherthing);
            window_mouse_move(me, cx, cy);
            return 0;
        }
        case WM_MOVE:
        case WM_SIZE: {
            GetClientRect(me->window, &me->client_rect);

            POINT a;
            POINT b;

            a.x = me->client_rect.left;
            a.y = me->client_rect.top;
            b.x = me->client_rect.right;
            b.y = me->client_rect.bottom;

            ClientToScreen(me->window, &a);
            ClientToScreen(me->window, &b);
            
            SetRect(&me->client_rect, a.x, a.y, b.x, b.y);

            window_resized(me, me->client_rect.right - me->client_rect.left, me->client_rect.bottom - me->client_rect.top);
            return 0;
        }
        case WM_CHAR: {
            window_keypress(me, thing, 0);
            return 0;
        }
        case WM_TIMER: {
            me->handle_bg_tasks(me);
            return 0;
        }
        default:
//            printf(FMT_WARN("got unknown wm event 0x%04x\n"), message);
            return DefWindowProc(window, message, thing, otherthing);
    }
}
#endif

window_t* window_init() {
    window_t* window = (window_t*)malloc(sizeof(window_t));

#ifdef WIN32
    window->instance                   = GetModuleHandle(0);
    window->window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window->window_class.lpfnWndProc   = &window_proc;
    window->window_class.cbClsExtra    = 0;
    window->window_class.cbWndExtra    = 0;
    window->window_class.hInstance     = window->instance;
    window->window_class.hIcon         = LoadIcon(0, IDI_APPLICATION);
    window->window_class.hCursor       = LoadCursor(0, IDC_ARROW);
    window->window_class.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    window->window_class.lpszMenuName  = 0;
    window->window_class.lpszClassName = "IRCRebornMain";
    window->class_thing                = RegisterClass(&window->window_class);
    
    window->window = CreateWindowEx(
        0,
        "IRCRebornMain",
        "IRCReborn",
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window->instance,
        0
    );
#else
    window->display = XOpenDisplay(0);

    // connect to xorg
    window->connection = XGetXCBConnection(window->display);
    
    // get a gc id
    window->gc     = xcb_generate_id(window->connection);
    
    // get a window id
    window->window = xcb_generate_id(window->connection);

    // get a cmap id
    window->cmap = xcb_generate_id(window->connection);

    // get the screen
    window->screen = xcb_setup_roots_iterator(xcb_get_setup(window->connection)).data;

    // make the gcontext mask
    uint32_t gcmaskv[2] = { 
        window->screen->black_pixel,
        0
    };
    
    // make the gcontext
    xcb_create_gc(
        window->connection,
        window->gc,
        window->screen->root,
        XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
        gcmaskv
    );
    
    // window mask
    uint32_t wmaskv[2] = {
        // bg color
        window->screen->white_pixel,
        // things to listen for
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE
    };

    // make the sexy window
    xcb_create_window(
        window->connection,
        XCB_COPY_FROM_PARENT,
        window->window,
        window->screen->root,
        0, 0, 400, 200, // xpos, ypos, width, height
        0, // this is the window managers problem now
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        window->screen->root_visual,
        XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
        wmaskv
    );
    
    // make the cmap
    xcb_create_colormap(
        window->connection,
        XCB_COLORMAP_ALLOC_NONE,
        window->cmap,
        window->window,
        window->screen->root_visual
    );

    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        SSTRLEN("IRCReborn"),
        "IRCReborn"
    );

    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        XCB_ATOM_WM_CLASS,
        XCB_ATOM_STRING,
        8,
        SSTRLEN("IRCReborn"),
        "IRCReborn"
    );

    font_request_t* fr = (font_request_t*)malloc(sizeof(font_request_t));
    fr->want_slant   = 1;
    fr->slant        = FSEARCH_SLANT_UPRIGHT;
    fr->want_style   = 1;
    fr->style        = FSEARCH_STYLE_NONE;
    fr->want_resx    = 1;
    fr->resx         = 75;
    fr->want_resy    = 1;
    fr->resy         = 100;
    fr->want_spacing = 1;
    fr->spacing      = FSEARCH_SPACING_MONO;
    window->main_font = request_font(window->connection, fr);
    
    free(fr);
#endif

    if (windows == 0) {
        windows = (window_t**)malloc(sizeof(void*));
    }

    window->height = 0;
    window->width = 0;
    
    window->widget_count = 0;
    window->widgets = (widget_t**)malloc(1);
    window->should_exit = 0;
    window->handle_bg_tasks = &__DEFAULT_window_handle_bg_tasks;
    window->resized = __DEFAULT_window_handle_resize;

    windows = (window_t**)realloc(windows, sizeof(void*) * (window_count + 1));

    windows[window_count] = window;
    window_count++;
    window_count_2++;

    window->pending_free = 0;

    return window;
}

void window_set_type(window_t* window, int type) {
#ifdef WIN32

#else
    xcb_intern_atom_cookie_t wmtypecookie       = xcb_intern_atom(window->connection, 0, 19, "_NET_WM_WINDOW_TYPE");
    xcb_intern_atom_reply_t* wmtype             = xcb_intern_atom_reply(window->connection, wmtypecookie, 0);
    xcb_intern_atom_cookie_t wmtypedialogcookie = xcb_intern_atom(window->connection, 0, 26, "_NET_WM_WINDOW_TYPE_DIALOG");
    xcb_intern_atom_reply_t* wmtypedialog       = xcb_intern_atom_reply(window->connection, wmtypedialogcookie, 0);
    
    if (type == WINDOW_WM_TYPE_DIALOG) {
        xcb_change_property(window->connection, XCB_PROP_MODE_REPLACE, window->window, wmtype->atom, 4, 32, 1, &wmtypedialog->atom);
    }

    free(wmtype);
    free(wmtypedialog);
#endif
}

void window_set_size(window_t* window, int width, int height) {
#ifdef WIN32

#else
    uint16_t propd = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    uint32_t propv[2] = {
        width,
        height
    };

    xcb_configure_window(
        window->connection, 
        window->window, 
        propd, propv
    );
#endif
}

void window_display(window_t* window, int all) {
#ifdef WIN32
    ShowWindow(window->window, SW_SHOWDEFAULT);
    UpdateWindow(window->window);

    MSG* msg = (MSG*)malloc(sizeof(MSG));
    int  ret;

    SetTimer(window->window, 0, 10, 0);

    while (ret = GetMessage(msg, 0, 0, 0)) {
        if (ret == -1) {
            logger_log(CHANNEL_FATL, "something broke\n");
        } else {
            TranslateMessage(msg);
            DispatchMessage(msg);
            if (window->should_exit) {
                break;
            }
        }
    }
#else

    // see https://marc.info/?l=freedesktop-xcb&m=129381953404497 for the close solution

    // this bullshit just for a close event
    xcb_intern_atom_cookie_t protocookie = xcb_intern_atom(window->connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* proto = xcb_intern_atom_reply(window->connection, protocookie, 0);
    xcb_intern_atom_cookie_t deletecookie = xcb_intern_atom(window->connection, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t* wm_delete = xcb_intern_atom_reply(window->connection, deletecookie, 0);
    xcb_change_property(window->connection, XCB_PROP_MODE_REPLACE, window->window, proto->atom, 4, 32, 1, &wm_delete->atom);
    
    // show the window
    xcb_map_window(window->connection, window->window);
    
    // flush the io buffer
    xcb_flush(window->connection);

    // main loop
    while (!window->should_exit) {
        for (int i = 0; i < window_count; i++) {
            window_t* window = windows[i];
            xcb_generic_event_t* e;

            while ((e = xcb_poll_for_event(window->connection))) {
                switch (e->response_type & 0x7f) {
                    // repaint
                    case XCB_EXPOSE: {
                        unused xcb_expose_event_t* event = (xcb_expose_event_t*)e;
                        window_paint(window);
                        break;
                    }
                    case XCB_CLIENT_MESSAGE: {
                        // client messages are fun to deal with
                        unused xcb_client_message_event_t* event = (xcb_client_message_event_t*)e;
                        
                        int type = event->data.data32[0];
                        
                        // cant use a switch because we get the value at runtime
                        if (type == wm_delete->atom) {
                            window_close(window);
                        } else {
                            logger_log(CHANNEL_DBUG, "got unknown message from the window manager, ignoring (%i)\n", type);
                        }

                        break;
                    }
                    case XCB_CONFIGURE_NOTIFY: {
                        unused xcb_configure_notify_event_t* event = (xcb_configure_notify_event_t*)e;
                        window_resized(window, event->width, event->height);                    
                        break;
                    }
                    case XCB_MOTION_NOTIFY: {
                        unused xcb_motion_notify_event_t* event = (xcb_motion_notify_event_t*)e;
                        window_mouse_move(window, event->event_x, event->event_y);
                        break;
                    }
                    case XCB_BUTTON_PRESS: {
                        unused xcb_button_press_event_t* event = (xcb_button_press_event_t*)e;
                        
                        if (event->detail == XCB_BUTTON_INDEX_1) {
                            window_left_mouse_down(window, event->event_x, event->event_y);
                        } else if (event->detail == XCB_BUTTON_INDEX_4 || event->detail == XCB_BUTTON_INDEX_5) {

                        } else {
                            logger_log(CHANNEL_DBUG, "got unknown button press type %i\n", event->detail);
                        }
                        
                        break;
                    }
                    case XCB_BUTTON_RELEASE: {
                        unused xcb_button_release_event_t* event = (xcb_button_release_event_t*)e;
                        
                        if (event->detail == XCB_BUTTON_INDEX_1) {
                            window_left_mouse_up(window, event->event_x, event->event_y);
                        } else if (event->detail == XCB_BUTTON_INDEX_4) {
                            window_scroll_up(window, event->event_x, event->event_y);
                        } else if (event->detail == XCB_BUTTON_INDEX_5) {
                            window_scroll_down(window, event->event_x, event->event_y);
                        } else {
                            logger_log(CHANNEL_DBUG, "got unknown button press type %i\n", event->detail);
                        }

                        break;
                    }
                    case XCB_KEY_PRESS: {
                        unused xcb_key_press_event_t* event = (xcb_key_press_event_t*)e;

                        int keysym_count;
                        KeySym *ks = XGetKeyboardMapping(
                            window->display,
                            event->detail,
                            1,
                            &keysym_count
                        );

                        // https://tronche.com/gui/x/xlib/input/keyboard-encoding.html
                        if (event->state & XCB_MOD_MASK_LOCK) {
                            if (event->state & XCB_MOD_MASK_SHIFT) {
                                window_keypress(window, ks[0], event->state);
                            } else {
                                window_keypress(window, ks[1], event->state);
                            }
                        } else if (event->state & XCB_MOD_MASK_SHIFT) {
                            window_keypress(window, ks[1] ? ks[1] : ks[0], event->state);
                        } else {
                            window_keypress(window, ks[0] ? ks[0] : ks[1], event->state);
                        }

                        
                        XFree(ks);

                        break;
                    }
                    case XCB_KEY_RELEASE: {
                        break;
                    }
                    default: {
                        logger_log(CHANNEL_DBUG, "got unknown message from xcb %i\n", e->response_type & ~0x80);
                        break;
                    }
                }


                free(e);
            }

            window->handle_bg_tasks(window);
        }
    }

    free(proto);
    free(wm_delete);
#endif
}

void window_free(window_t* window) {
#ifndef WIN32
    XCloseDisplay(window->display);
#else
    DestroyWindow(window->window);
#endif
    free(window->widgets);
    free(window);
    window_count_2--;
    if (window_count_2 == 0) {
        window_count = 0;
        free(windows);
    }
}

void window_add_widget(window_t* window, widget_t* widget) {
    window->widgets = (widget_t**)realloc(window->widgets, (window->widget_count+1) * sizeof(void*));
    window->widgets[window->widget_count] = widget;
    widget->hovered = 0;
    window->widget_count++;    
}

void window_remove_widget(window_t* window, widget_t* widget) {

}

// this is useless and only exists for consistency
void window_set_bg(window_t* window, rgba_t value) {
    window->bg_color = value;
}

void window_set_focus(window_t* window, widget_t* widget) {
    window->focused = widget;
}