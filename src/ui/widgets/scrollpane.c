#include <ui/widgets/scrollpane.h>
#include <ui/widget.h>
#include <ui/window.h>
#include <common/util.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>

void scroll_pane_draw(widget_t*, window_t*);
void scroll_pane_mousein(widget_t*, window_t*);
void scroll_pane_mouseout(widget_t*, window_t*);
void scroll_pane_clicked(widget_t*, window_t*, int, int);
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

    SetRect(
        rect,
        widget->x + widget->width - 20,
        widget->y + 20 + ceil(tpos),
        widget->x + widget->width,
        widget->y + 20 + ceil(tpos) + ceil(tsize)
    );
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    EndPaint(window->window, hi);

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

    free(hi);
    free(rect);
}

void scroll_pane_clicked(widget_t* widget, window_t* window, int x, int y) {
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
    }

    widget->draw(widget, window);
}

void scroll_pane_mousein(widget_t* widget, window_t* window) {
    
}

void scroll_pane_mouseout(widget_t* widget, window_t* window) {
    
}
