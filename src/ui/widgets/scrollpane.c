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

#ifdef WIN32
#include <windows.h>
#else

#endif

void scroll_pane_draw(widget_t*, window_t*);
void scroll_pane_mousein(widget_t*, window_t*);
void scroll_pane_mouseout(widget_t*, window_t*);
int  scroll_pane_clicked(widget_t*, window_t*, int, int);
void scroll_pane_mousedown(widget_t* widget, window_t* window, int x, int y);
void scroll_pane_mouseup(widget_t* widget, window_t* window, int x, int y);
void scroll_pane_mousemove(widget_t* widget, window_t* window, int x, int y);

widget_t* scroll_pane_init() {
    widget_t* widget    = widget_init();
    scroll_pane_t* pane = malloc(sizeof(scroll_pane_t));

    pane->widget           = widget;
    pane->pos              = 0;
    pane->itemc            = 0;
    pane->items            = malloc(1);
    pane->thumb_dragging   = 0;

    widget->extra_data     = pane;
    widget->draw           = &scroll_pane_draw;
    widget->mousein        = &scroll_pane_mousein;
    widget->mouseout       = &scroll_pane_mouseout;
    widget->clicked        = &scroll_pane_clicked;
    widget->mousedown      = &scroll_pane_mousedown;
    widget->mouseup        = &scroll_pane_mouseup;
    widget->mousemove      = &scroll_pane_mousemove;

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
    
    uint32_t maskd    = XCB_GC_FOREGROUND;
    uint32_t maskv[1] = { 
        window->screen->white_pixel
    };

    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);

    maskv[0] = window->screen->black_pixel;
    
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    rect[1].x      = widget->x + widget->width - 20;
    rect[1].y      = widget->y;
    rect[1].width  = 20;
    rect[1].height = 20;
    rect[2].x      = widget->x + widget->width  - 20;
    rect[2].y      = widget->y + widget->height - 20;
    rect[2].width  = 20;
    rect[2].height = 20;
    rect[3].x      = widget->x + widget->width - 20;
    rect[3].y      = widget->y + 20;
    rect[3].width  = 20;
    rect[3].height = ceil(tpos) + ceil(tsize);

    xcb_poly_rectangle(window->connection, window->window, window->gc, 4, rect);
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

void scroll_pane_mousedown(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;
    
    if (widget->x + widget->width - 20 < x) {
        if (widget->y + 20 < y && y < widget->y + widget->height - 20) {
            scroll_pane->thumb_dragging = 1;
            scroll_pane->thumb_drag_src = y - widget->y - 20 + scroll_pane->pos;
        }
    }
}

void scroll_pane_mouseup(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    scroll_pane->thumb_dragging = 0;

    widget->draw(widget, window);
}

void scroll_pane_mousemove(widget_t* widget, window_t* window, int x, int y) {
    scroll_pane_t* scroll_pane = widget->extra_data;

    if (scroll_pane->thumb_dragging) {
        int a = ceil((-y + scroll_pane->thumb_drag_src + widget->y + 20) * (double)(scroll_pane->csize)) / (widget->height-40);
        a = a < 0 ? a : 0;
        a = a > -scroll_pane->csize + (scroll_pane->csize / (widget->height - 40)) * widget->height ? a : -scroll_pane->csize + (scroll_pane->csize / (widget->height - 40)) * widget->height;
        scroll_pane->pos = a;
        if (scroll_pane->pos != scroll_pane->prev_pos) {
            printf("%i\n", scroll_pane->pos);
            scroll_pane->prev_pos = scroll_pane->pos;
            widget->draw(widget, window);
        }
    }
}

void scroll_pane_mousein(widget_t* widget, window_t* window) {
    
}

void scroll_pane_mouseout(widget_t* widget, window_t* window) {
    
}
