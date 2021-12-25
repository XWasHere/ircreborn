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

#include <ui/widgets/frame.h>
#include <ui/widget.h>
#include <stdlib.h>

void frame_draw(widget_t* widget, window_t* window) {
    frame_t* frame = widget->extra_data;

    if (frame->bg_color.a != 0) {
#ifndef WIN32
        xcb_rectangle_t rect;

        rect.height = frame->widget->height;
        rect.width  = frame->widget->width;
        rect.x      = frame->widget->x;
        rect.y      = frame->widget->y;

        xcb_alloc_color_reply_t* r = xcb_alloc_color_reply(
            window->connection,
            xcb_alloc_color(
                window->connection,
                window->cmap,
                frame->bg_color.r << 8,
                frame->bg_color.g << 8,
                frame->bg_color.b << 8
            ),
            NULL
        );

        uint32_t hi[] = {
            r->pixel
        };

        xcb_change_gc(
            window->connection,
            window->gc,
            XCB_GC_FOREGROUND,
            hi
        );

        xcb_poly_fill_rectangle(
            window->connection,
            window->window,
            window->gc,
            1,
            &rect
        );
#endif
    }

    for (int i = 0; i < frame->item_count; i++) {
        frame_managed_t* item = frame->items[i];
        
        item->widget->x = item->x + widget->x;
        item->widget->y = item->y + widget->y;

        item->widget->draw(item->widget, window);
    }
}

int frame_click(widget_t* widget, window_t* window, int x, int y) {
    frame_t* frame = widget->extra_data;

    for (int i = 0; i < frame->item_count; i++) {
        frame_managed_t* item = frame->items[i];
        widget_t* target = item->widget;
        if (target->x <= x && x <= target->x + target->width && target->y <= y && y <= target->y + target->height) {
            target->clicked(target, window, x, y);
        }
    }

    return 1;
}

widget_t* frame_init() {
    frame_t* frame = malloc(sizeof(frame_t));

    frame->item_count = 0;
    frame->items      = malloc(1);
    frame->widget     = widget_init();

    frame->widget->extra_data = frame;
    frame->widget->draw       = frame_draw;
    frame->widget->clicked    = frame_click;

    return frame->widget;
}

frame_managed_t* frame_add_item(frame_t* frame, widget_t* widget) {
    frame_managed_t* managed = malloc(sizeof(frame_managed_t));
    
    managed->widget = widget;
    
    frame->item_count++;
    frame->items = realloc(frame->items, sizeof(void*) * frame->item_count);
    frame->items[frame->item_count - 1] = managed;
    
    return managed;
}

void frame_set_color(widget_t* widget, int type, rgba_t value) {
    frame_t* frame = widget->extra_data;

    if (type == FRAME_COLOR_BG) {
        frame->bg_color = value;
    }
}

void frame_free(widget_t* widget) {
    frame_t* frame = widget->extra_data;

    for (int i = 0; i < frame->item_count; i++) {
        free(frame->items[i]);
    }
    
    free(frame->items);
    free(frame);
    widget_free(widget);
}