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

#include <ui/widgets/scrollpane.h>
#include <ui/widget.h>
#include <ui/window.h>
#include <common/util.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else

#endif

void scroll_pane_draw(widget_t*, window_t*);
int  scroll_pane_clicked(widget_t*, window_t*, int, int);
int  scroll_pane_mousedown(widget_t* widget, window_t* window, int x, int y);
int  scroll_pane_mouseup(widget_t* widget, window_t* window, int x, int y);
int  scroll_pane_mousemove(widget_t* widget, window_t* window, int x, int y);
int  scroll_pane_mouseout(widget_t*, window_t*);
int  scroll_pane_scroll_up(widget_t*, window_t*);
int  scroll_pane_scroll_down(widget_t*, window_t*);

widget_t* scroll_pane_init() {
    widget_t* widget    = widget_init();
    scroll_pane_t* pane = malloc(sizeof(scroll_pane_t));

    pane->widget           = widget;
    pane->pos              = 0;
    pane->prev_pos         = 0;
    pane->itemc            = 0;
    pane->items            = malloc(1);
    pane->thumb_dragging   = 0;

    widget->extra_data     = pane;
    widget->draw           = &scroll_pane_draw;
    widget->clicked        = &scroll_pane_clicked;
    widget->mousedown      = &scroll_pane_mousedown;
    widget->mouseup        = &scroll_pane_mouseup;
    widget->mousemove      = &scroll_pane_mousemove;
    widget->mouseout       = &scroll_pane_mouseout;
    widget->scroll_up      = &scroll_pane_scroll_up;
    widget->scroll_down    = &scroll_pane_scroll_down;

    return pane->widget;
}

scroll_pane_item_t* scroll_pane_add_item(widget_t* scrollpane, widget_t* widget) {
    scroll_pane_item_t* item = malloc(sizeof(scroll_pane_item_t));
    item->widget = widget;
    item->x = 0;
    item->y = 0;
    scroll_pane_t* pane = scrollpane->extra_data;
    pane->itemc++;
    pane->items = realloc(pane->items, sizeof(void*) * (pane->itemc + 1));
    pane->items[pane->itemc - 1] = item;

    return item;
}

void scroll_pane_draw(widget_t* widget, window_t* window) {
    scroll_pane_t* sp = widget->extra_data;

    int csize = 0;
    for (int i = 0; i < sp->itemc; i++) {
        int a = sp->items[i]->y + sp->items[i]->widget->height;
        if (a > csize) csize = a;
    }

    sp->csize = csize;

    // thumb position.
    double tpos;
    if (csize >= widget->height) {
        tpos = (-sp->pos/(double)(csize))*(widget->height-40);
    } else {
        tpos = 0;
    }

    // thumb size
    double tsize;
    if (csize >= widget->height) {
        tsize = (widget->height/(double)(csize))*(widget->height-40);
    } else {
        tsize = widget->height - 40;
    }

    sp->thumb_pos = ceil(tpos);

#ifdef WIN32
    PAINTSTRUCT* hi   = malloc(sizeof(PAINTSTRUCT));
    RECT*        rect = malloc(sizeof(RECT));

    // gotta please the wm
    SetRect(
        rect,
        widget->x,
        widget->y,
        widget->x + widget->width,
        widget->y + widget->height
    );
    InvalidateRect(window->window, rect, 1);

    BeginPaint(window->window, hi);
    
    SetRect(
        rect,
        widget->x + widget->width - 20,
        widget->y,
        widget->x + widget->width,
        widget->y + 20
    );
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        widget->x + widget->width - 20,
        widget->y + widget->height - 20,
        widget->x + widget->width,
        widget->y + widget->height
    );
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        widget->x + widget->width - 20,
        widget->y + 20 + ceil(tpos),
        widget->x + widget->width,
        widget->y + 20 + ceil(tpos) + ceil(tsize)
    );
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    EndPaint(window->window, hi);
#else
    xcb_rectangle_t* rect = malloc(sizeof(xcb_rectangle_t) * 4);

    rect[0].x      = widget->x;
    rect[0].y      = widget->y;
    rect[0].height = widget->height;
    rect[0].width  = widget->width;
    
    xcb_alloc_color_reply_t* r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            sp->bg_color.r << 8,
            sp->bg_color.g << 8,
            sp->bg_color.b << 8
        ),
        NULL
    );

    uint32_t maskd    = XCB_GC_FOREGROUND;
    uint32_t maskv[1] = { 
        r->pixel
    };

    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);

    
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            sp->track_color.r << 8,
            sp->track_color.g << 8, 
            sp->track_color.b << 8
        ),
        NULL
    );
    maskv[0] = r->pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    rect[0].x      = widget->x + widget->width - 20;
    rect[0].y      = widget->y;
    rect[0].width  = 20;
    rect[0].height = widget->height;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            sp->button_color.r << 8,
            sp->button_color.g << 8,
            sp->button_color.b << 8
        ),
        NULL
    );
    maskv[0] = r->pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    rect[0].x      = widget->x + widget->width - 20;
    rect[0].y      = widget->y;
    rect[0].width  = 20;
    rect[0].height = 20;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            sp->button_color.r << 8,
            sp->button_color.g << 8,
            sp->button_color.b << 8
        ),
        NULL
    );
    maskv[0] = r->pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    rect[0].x      = widget->x + widget->width  - 20;
    rect[0].y      = widget->y + widget->height - 20;
    rect[0].width  = 20;
    rect[0].height = 20;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            sp->thumb_color.r << 8,
            sp->thumb_color.g << 8,
            sp->thumb_color.b << 8
        ),
        NULL
    );
    maskv[0] = r->pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    rect[0].x      = widget->x + widget->width - 20;
    rect[0].y      = widget->y + 20 + ceil(tpos);
    rect[0].width  = 20;
    rect[0].height = csize <= widget->height ? 0 : ceil(tsize);
//    printf("%i %i\n", csize, widget->height);
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);

    xcb_flush(window->connection);
#endif

    for (int i = 0; i < sp->itemc; i++) {
        scroll_pane_item_t* item = sp->items[i];
        item->widget->x = item->x + widget->x;
        if (csize >= widget->height) {
            item->widget->y = item->y + widget->y + sp->pos;
        } else {
            item->widget->y = item->y + widget->y;
        }
        if (item->widget->y >= widget->y && item->widget->y + item->widget->height <= widget->y + widget->height) {
            item->widget->draw(item->widget, window);
        }
    }

    free(rect);
#ifdef WIN32
    free(hi);
#endif
}

void scroll_pane_set_color(widget_t* widget, int type, rgba_t value) {
    scroll_pane_t* pane = widget->extra_data;

    if (type == SCROLLPANE_COLOR_BG) {
        pane->bg_color = value;
    } else if (type == SCROLLPANE_COLOR_TRACK) {
        pane->track_color = value;
    } else if (type == SCROLLPANE_COLOR_THUMB) {
        pane->thumb_color = value;
    } else if (type == SCROLLPANE_COLOR_BUTTON) {
        pane->button_color = value;
    }
}

int scroll_pane_clicked(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* pane = widget->extra_data;

    if (widget->x + widget->width - 20 < x) {
        if (widget->y < y && y < widget->y + 20) {
            pane->pos += 10;
        } else if (widget->y + widget->height - 20 < y && y < widget->y + widget->height) {
            pane->pos -= 10;
        }
        widget->draw(widget, window);
    } else {
        for (int i = 0; i < pane->itemc; i++) {
            scroll_pane_item_t* item = pane->items[i];
            widget_t* target = item->widget;

            if (target->x <= x && x <= target->x + target->width && target->y <= y && y <= target->y + target->height) {
                target->clicked(target, window, x, y);
            }
        }
    }

    return 1;
}

int scroll_pane_mousedown(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;
    
    if (widget->x + widget->width - 20 < x) {
        if (widget->y + 20 < y && y < widget->y + widget->height - 20) {
            scroll_pane->thumb_dragging = 1;
            scroll_pane->thumb_drag_src = y - widget->y - 20 + scroll_pane->pos;
        }
    }

    return 1;
}

int scroll_pane_mouseup(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    scroll_pane->thumb_dragging = 0;

    widget->draw(widget, window);

    return 1;
}

int scroll_pane_mouseout(widget_t* widget, window_t* window) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    scroll_pane->thumb_dragging = 0;

    return 1;
}

int scroll_pane_mousemove(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    if (scroll_pane->thumb_dragging) {
        int a = ceil((-y + scroll_pane->thumb_drag_src + widget->y + 20) * (double)(scroll_pane->csize)) / (widget->height-40);
        a = a < 0 ? a : 0;
        a = a > -scroll_pane->csize + (scroll_pane->csize / (widget->height - 40)) * widget->height ? a : -scroll_pane->csize + (scroll_pane->csize / (widget->height - 40)) * widget->height;
        scroll_pane->pos = a;
        if (scroll_pane->pos != scroll_pane->prev_pos) {
            scroll_pane->prev_pos = scroll_pane->pos;
            widget->draw(widget, window);
        }
    }

    return 1;
}

int scroll_pane_scroll_up(widget_t* widget, window_t* window) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    scroll_pane->pos += 10;
    widget->draw(widget, window);

    return 1;
}

int scroll_pane_scroll_down(widget_t* widget, window_t* window) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    scroll_pane->pos -= 10;
    widget->draw(widget, window);
    
    return 1;
}

void scroll_pane_free(widget_t* widget) {
    scroll_pane_t* scrollpane = widget->extra_data;

    for (int i = 0; i < scrollpane->itemc; i++) {
        free(scrollpane->items[i]);
    }

    widget_free(widget);
    free(scrollpane->items);
    free(scrollpane);
}