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

#include <ui/widgets/frame.h>
#include <ui/uitypes.h>
#include <stdlib.h>

void frame_t::draw() {
    if (this->bg_color.a != 0) {
#ifdef WIN32
        PAINTSTRUCT* ps = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
        RECT* rect = (RECT*)malloc(sizeof(RECT));

        SetRect(rect, this->x, this->y, this->x + this->width, this->y + this->height);
        InvalidateRect(this->window->window, rect, 1);
        BeginPaint(this->window->window, ps);

        SetBkColor(ps->hdc, W32RGBAC(this->bg_color));
        Rectangle(ps->hdc, rect->left, rect->top, rect->right, rect->bottom);

        EndPaint(this->window->window, ps);

        free(rect);
        free(ps);
#else
        xcb_rectangle_t rect;

        rect.height = this->height;
        rect.width  = this->width;
        rect.x      = this->x;
        rect.y      = this->y;

        xcb_alloc_color_reply_t* r = xcb_alloc_color_reply(
            this->window->connection,
            xcb_alloc_color(
                this->window->connection,
                this->window->cmap,
                this->bg_color.r << 8,
                this->bg_color.g << 8,
                this->bg_color.b << 8
            ),
            NULL
        );

        uint32_t hi[] = {
            r->pixel
        };

        xcb_change_gc(
            this->window->connection,
            this->window->gc,
            XCB_GC_FOREGROUND,
            hi
        );

        xcb_poly_fill_rectangle(
            this->window->connection,
            this->window->window,
            this->window->gc,
            1,
            &rect
        );

        free(r);
#endif
    }

    for (int i = 0; i < this->item_count; i++) {
        frame_managed_t* item = this->items[i];
        
        item->widget->x = item->x + this->x;
        item->widget->y = item->y + this->y;

        item->widget->draw();
    }
}

int frame_t::clicked(int x, int y) {
    for (int i = 0; i < this->item_count; i++) {
        frame_managed_t* item = this->items[i];
        widget_t* target = item->widget;
        if (target->x <= x && x <= target->x + target->width && target->y <= y && y <= target->y + target->height) {
            target->clicked(x, y);
        }
    }

    return 1;
}

frame_t::frame_t() {
    this->item_count = 0;
    this->items      = (frame_managed_t**)malloc(1);
}

void* frame_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void frame_t::operator delete(void* address) {
    free(address);
}

frame_managed_t* frame_t::add_item(widget_t* widget) {
    frame_managed_t* managed = (frame_managed_t*)malloc(sizeof(frame_managed_t));
    
    managed->widget = widget;
    
    this->item_count++;
    this->items = (frame_managed_t**)realloc(this->items, sizeof(void*) * this->item_count);
    this->items[this->item_count - 1] = managed;
    
    return managed;
}

frame_t::~frame_t() {
    for (int i = 0; i < this->item_count; i++) {
        delete this->items[i]->widget;
        free(this->items[i]);
    }

    free(this->items);
}