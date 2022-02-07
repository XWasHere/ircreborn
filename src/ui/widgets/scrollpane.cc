/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2022 IRCReborn Devs

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
#include <ui/uitypes.h>
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

scroll_pane_t::scroll_pane_t() {
    this->pos              = 0;
    this->prev_pos         = 0;
    this->itemc            = 0;
    this->items            = (scroll_pane_item_t**)malloc(1);
    this->thumb_dragging   = 0;
    this->ticks_for_update = 10;
    this->ticks            = 0;
}

scroll_pane_t::~scroll_pane_t() {
    for (int i = 0; i < this->itemc; i++) {
        if (this->items[i] != 0) {
            delete this->items[i]->widget;
            free(this->items[i]);
            this->items[i] = 0;
        }
    }

    free(this->items);
}

void* scroll_pane_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void scroll_pane_t::operator delete(void* address) {
    free(address);
}

scroll_pane_item_t* scroll_pane_t::add_item(widget_t* widget) {
    scroll_pane_item_t* item = (scroll_pane_item_t*)malloc(sizeof(scroll_pane_item_t));
    item->widget = widget;
    item->x = 0;
    item->y = 0;

    this->itemc++;
    this->items = (scroll_pane_item_t**)realloc(this->items, sizeof(void*) * (this->itemc + 1));
    this->items[this->itemc - 1] = item;

    widget->window_set(this->window);
    
    return item;
}

void scroll_pane_t::draw() {
    int csize = 0;
    for (int i = 0; i < this->itemc; i++) {
        int a = this->items[i]->y + this->items[i]->widget->height;
        if (a > csize) csize = a;
    }

    this->csize = csize;

    // thumb position.
    double tpos;
    if (csize >= this->height) {
        tpos = (-this->pos/(double)(csize))*(this->height-40);
    } else {
        tpos = 0;
    }

    // thumb size
    double tsize;
    if (csize >= this->height) {
        tsize = (this->height/(double)(csize))*(this->height-40);
    } else {
        tsize = this->height - 40;
    }

    this->thumb_pos = ceil(tpos);

#ifdef WIN32
    PAINTSTRUCT* hi   = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
    RECT*        rect = (RECT*)malloc(sizeof(RECT));

    // gotta please the wm
    SetRect(
        rect,
        this->x,
        this->y,
        this->x + this->width,
        this->y + this->height
    );
    InvalidateRect(window->window, rect, 1);

    BeginPaint(window->window, hi);
    
    SelectObject(hi->hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hi->hdc, W32RGBAC(this->bg_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        this->x + this->width - 20,
        this->y,
        this->x + this->width,
        this->y + this->height
    );
    SetDCBrushColor(hi->hdc, W32RGBAC(this->track_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        this->x + this->width - 20,
        this->y,
        this->x + this->width,
        this->y + 20
    );
    SetDCBrushColor(hi->hdc, W32RGBAC(this->button_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        this->x + this->width - 20,
        this->y + this->height - 20,
        this->x + this->width,
        this->y + this->height
    );
    SetDCBrushColor(hi->hdc, W32RGBAC(this->button_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    SetRect(
        rect,
        this->x + this->width - 20,
        this->y + 20 + ceil(tpos),
        this->x + this->width,
        this->y + 20 + ceil(tpos) + ceil(tsize)
    );
    SetDCBrushColor(hi->hdc, W32RGBAC(this->thumb_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    EndPaint(window->window, hi);
#else
    xcb_rectangle_t* rect = (xcb_rectangle_t*)malloc(sizeof(xcb_rectangle_t) * 4);

    rect[0].x      = this->x;
    rect[0].y      = this->y;
    rect[0].height = this->height;
    rect[0].width  = this->width;
    
    xcb_alloc_color_reply_t* r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->bg_color.r << 8,
            this->bg_color.g << 8,
            this->bg_color.b << 8
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
    free(r);
    
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->track_color.r << 8,
            this->track_color.g << 8, 
            this->track_color.b << 8
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
    rect[0].x      = this->x + this->width - 20;
    rect[0].y      = this->y;
    rect[0].width  = 20;
    rect[0].height = this->height;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    free(r);
    
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->button_color.r << 8,
            this->button_color.g << 8,
            this->button_color.b << 8
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
    rect[0].x      = this->x + this->width - 20;
    rect[0].y      = this->y;
    rect[0].width  = 20;
    rect[0].height = 20;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    free(r);
    
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->button_color.r << 8,
            this->button_color.g << 8,
            this->button_color.b << 8
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
    rect[0].x      = this->x + this->width  - 20;
    rect[0].y      = this->y + this->height - 20;
    rect[0].width  = 20;
    rect[0].height = 20;
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    free(r);
    
    r = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->thumb_color.r << 8,
            this->thumb_color.g << 8,
            this->thumb_color.b << 8
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
    rect[0].x      = this->x + this->width - 20;
    rect[0].y      = this->y + 20 + ceil(tpos);
    rect[0].width  = 20;
    rect[0].height = csize <= this->height ? 0 : ceil(tsize);
//    printf("%i %i\n", csize, widget->height);
    xcb_poly_fill_rectangle(window->connection, window->window, window->gc, 1, rect);
    free(r);

    xcb_flush(window->connection);
#endif

    for (int i = 0; i < this->itemc; i++) {
        scroll_pane_item_t* item = this->items[i];
        item->widget->x = item->x + this->x;
        if (csize >= this->height) {
            item->widget->y = item->y + this->y + this->pos;
        } else {
            item->widget->y = item->y + this->y;
        }
        if (item->widget->y >= this->y && item->widget->y + item->widget->height <= this->y + this->height) {
            item->widget->draw();
        }
    }

    free(rect);
#ifdef WIN32
    free(hi);
#endif
}

int scroll_pane_t::clicked(int x, int y) {
    if (this->x + this->width - 20 < x) {
        if (this->y < y && y < this->y + 20) {
            this->pos += 10;
        } else if (this->y + this->height - 20 < y && y < this->y + this->height) {
            this->pos -= 10;
        }
        this->draw();
    } else {
        for (int i = 0; i < this->itemc; i++) {
            scroll_pane_item_t* item = this->items[i];
            widget_t* target = item->widget;

            if (target->x <= x && x <= target->x + target->width && target->y <= y && y <= target->y + target->height) {
                target->clicked(x, y);
            }
        }
    }

    return 1;
}

int scroll_pane_t::mousedown(int x, int y) {
    if (this->x + this->width - 20 < x) {
        if (this->y + 20 < y && y < this->y + this->height - 20) {
            this->thumb_dragging = 1;
            this->thumb_drag_src = y - this->y - 20 + this->pos;
        }
    }

    return 1;
}

int scroll_pane_t::mouseup(int x, int y) {
    this->thumb_dragging = 0;

    this->draw();

    return 1;
}

int scroll_pane_t::mouseout() {
    this->thumb_dragging = 0;
    
    return 1;
}

int scroll_pane_t::mousemove(int x, int y) {
    if (this->thumb_dragging) {
        // what the hell. for some reason the ui completely broke when i added this if statement.
        if (this->ticks >= this->ticks_for_update) {
            // what was i thinking when i wrote the old code for this
            int a = ceil((-y + this->thumb_drag_src + this->y + 20) * (double)(this->csize)) / (this->height-40);
            this->pos = a;
            if (this->pos != this->prev_pos) {
                if (this->pos < -this->csize + this->height + 10) {
                    this->pos = -this->csize + this->height;
                } else if (this->pos > -10) {
                    this->pos = 0;
                }

                this->prev_pos = this->pos;
                this->draw();
            }
            this->ticks = 0;
        }
        this->ticks++;
    }

    return 1;
}

int scroll_pane_t::scroll_up() {
    if (this->pos < -10) {
        this->pos += 10;
    } else {
        this->pos = 0;
    }

    this->draw();

    return 1;
}

int scroll_pane_t::scroll_down() {
    if (this->pos > -this->csize + this->height + 10) {
        this->pos -= 10;
    } else {
        this->pos = -this->csize + this->height;
    }

    this->draw();
    
    return 1;
}