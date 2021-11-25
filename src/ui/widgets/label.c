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

#include <ui/widget.h>
#include <ui/window.h>
#include <ui/widgets/label.h>
#ifndef WIN32 
#include <ui/util/font_search.h>
#endif
#include <stdlib.h>
#include <string.h>

void label_draw(widget_t* widget ,window_t* window) {
    label_t* label = widget->extra_data;

#ifdef WIN32
    PAINTSTRUCT* hi = malloc(sizeof(PAINTSTRUCT));
    RECT *rect = malloc(sizeof(RECT));
    
    SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);
    
    SetRect(rect, widget->x + 1, widget->y + 1, widget->x + widget->width - 1, widget->y + widget->height - 1);
    DrawText(hi->hdc, label->text, strlen(label->text), rect, 0);

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
#else
    // literally copied from button.c
    xcb_gcontext_t gc = xcb_generate_id(window->connection);

    uint32_t maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    uint32_t maskv[3] = {
        window->screen->black_pixel,
        window->screen->white_pixel,
        window->main_font
    };

    xcb_create_gc(
        window->connection,
        gc,
        window->window,
        maskd,
        maskv
    );

    xcb_image_text_8(
        window->connection,
        strlen(label->text),
        window->window,
        gc,
        widget->x + 3,
        widget->y + widget->height - 3,
        label->text
    );

    xcb_free_gc(window->connection, gc);
    
    xcb_rectangle_t *rect = malloc(sizeof(xcb_rectangle_t));
    
    rect->x = widget->x;
    rect->y = widget->y;
    rect->width = widget->width;
    rect->height = widget->height;
    
    xcb_poly_rectangle(
        window->connection,
        window->window,
        window->gc,
        1,
        rect
    );

    xcb_flush(window->connection);
    
    free(rect);
#endif
}

widget_t* label_init() {
    widget_t* label = widget_init();
    label_t*  lab   = malloc(sizeof(label_t));

    lab->text = 0;

    lab->widget = label;
    label->extra_data = lab;

    label->draw = &label_draw;
    
    return label;
}

void label_set_text(widget_t* widget, char* text) {
    label_t* label = widget->extra_data;
    char* buf = malloc(strlen(text) + 1);

    memset(buf, 0, strlen(text) + 1);
    strcpy(buf, text);
    label->text = buf;
}

void label_free(widget_t* widget) {
    label_t* label = widget->extra_data;

    free(label->text);
    free(label);
    widget_free(widget);
}