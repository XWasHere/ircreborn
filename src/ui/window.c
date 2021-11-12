#include <common/util.h>
#include <ui/widget.h>
#include <ui/window.h>
#include <windows.h> // ugh.
#include <stdio.h>

window_t** windows      = 0;
int        window_count = 0;

void __DEFAULT_window_handle_bg_tasks(window_t* a) {};

window_t* resolve_window(HWND a) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i]->window == a) return windows[i];
    }
}

LRESULT libui_window_proc(HWND window, UINT message, WPARAM thing, LPARAM otherthing) {
    window_t* me = resolve_window(window);
    switch (message) {
        case WM_CLOSE: {
            printf("got exit signal\n");
            me->should_exit = 1;
            return 0;
        }
        case WM_PAINT: {
            for (int i = 0; i < me->widget_count; i++) {
                widget_t* widget = me->widgets[i];
                widget->draw(widget, me);
            }

            return 0;
        }
        case WM_LBUTTONDOWN: {
            ClipCursor(&me->client_rect);
            me->mouse_left_down = 1; // the mouse button is down :catnod:
            int cx = LOWORD(otherthing); 
            int cy = HIWORD(otherthing);
            
            for (int i = 0; i < me->widget_count; i++) {
                widget_t* widget = me->widgets[i];
                if (widget->x < cx && cx < widget->x + widget->width && widget->y < cy && cy < widget->y + widget->height) {
                    widget->mousedown(widget, me, cx, cy);
                }
            }

            return 0;
        }
        case WM_LBUTTONUP: {
            int cx = LOWORD(otherthing); 
            int cy = HIWORD(otherthing);
            ClipCursor(0);
            me->mouse_left_down = 0;
            
            // this is a click B)
            for (int i = 0; i < me->widget_count; i++) {
                widget_t* widget = me->widgets[i];
                if (widget->x < cx && cx < widget->x + widget->width && widget->y < cy && cy < widget->y + widget->height) {
                    widget->mouseup(widget, me, cx, cy);
                    widget->clicked(widget, me, cx, cy);
                }
            }

            return 0;
        }
        case WM_MOUSEMOVE: {
            int cx      = LOWORD(otherthing); 
            int cy      = HIWORD(otherthing);
            for (int i = 0; i < me->widget_count; i++) {
                widget_t* widget = me->widgets[i];
                if (widget->x < cx && cx < widget->x + widget->width && widget->y < cy && cy < widget->y + widget->height) {
                    if (widget->hovered == 0) {
                        widget->hovered = 1;
                        widget->mousein(widget, me);
                    }
                    widget->mousemove(widget, me, cx, cy);
                } else {
                    if (widget->hovered == 1) {
                        widget->hovered = 0;
                        widget->mouseout(widget, me);
                    }
                }
            }
            return 0;
        }
        case WM_MOVE:
        case WM_SIZE: {
            GetClientRect(window, &me->client_rect);

            POINT a;
            POINT b;

            a.x = me->client_rect.left;
            a.y = me->client_rect.top;
            b.x = me->client_rect.right;
            b.y = me->client_rect.bottom;

            ClientToScreen(window, &a);
            ClientToScreen(window, &b);
            
            SetRect(&me->client_rect, a.x, a.y, b.x, b.y);

            return 0;
        }
        case WM_CHAR: {
            widget_t* focused = me->focused;
            
            if (focused == 0) return 0;

            focused->keypress(focused, me, thing);

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

window_t* window_init() {
    window_t* window = malloc(sizeof(window_t));

    window->instance                   = GetModuleHandle(0);
    window->window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window->window_class.lpfnWndProc   = &libui_window_proc;
    window->window_class.cbClsExtra    = 0;
    window->window_class.cbWndExtra    = 0;
    window->window_class.hInstance     = window->instance;
    window->window_class.hIcon         = LoadIcon(0, IDI_APPLICATION);
    window->window_class.hCursor       = LoadCursor(0, IDC_ARROW);
    window->window_class.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    window->window_class.lpszMenuName  = 0;
    window->window_class.lpszClassName = "LibUIDefault";
    window->class_thing                = RegisterClass(&window->window_class);
    
    window->window = CreateWindowEx(
        0,
        "LibUIDefault",
        "LibUI Window",
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

    if (windows == 0) {
        windows = malloc(sizeof(void*));
    }

    window->widget_count = 0;
    window->widgets = malloc(1);
    window->should_exit = 0;
    window->handle_bg_tasks = &__DEFAULT_window_handle_bg_tasks;
    
    windows = realloc(windows, sizeof(void*) * (window_count + 1));

    windows[window_count] = window;
    window_count++;

    return window;
}

void window_display(window_t* window) {
    ShowWindow(window->window, SW_SHOWDEFAULT);
    UpdateWindow(window->window);

    MSG* msg = malloc(sizeof(MSG));
    int  ret;

    SetTimer(window->window, 0, 10, 0);

    while (ret = GetMessage(msg, 0, 0, 0)) {
        if (ret == -1) {
            printf(FMT_FATL("something broke\n"));
        } else {
            TranslateMessage(msg);
            DispatchMessage(msg);
            if (window->should_exit) {
                printf("exiting\n");
                break;
            }
        }
    }
}

void window_add_widget(window_t* window, widget_t* widget) {
    window->widgets = realloc(window->widgets, (window->widget_count+1) * sizeof(void*));
    window->widgets[window->widget_count] = widget;
    widget->hovered = 0;
    window->widget_count++;    
}

void window_set_focus(window_t* window, widget_t* widget) {
    window->focused = widget;
}