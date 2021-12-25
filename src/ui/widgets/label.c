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

    SelectObject(hi->hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hi->hdc, W32RGBAC(label->bg_color));
    SetBkColor(hi->hdc, W32RGBAC(label->bg_color));
    SetTextColor(hi->hdc, W32RGBAC(label->text_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);
    
    SetRect(rect, widget->x + 1, widget->y + 1, widget->x + widget->width - 1, widget->y + widget->height - 1);
    DrawText(hi->hdc, label->text, strlen(label->text), rect, 0);

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
#else
    // literally copied from button.c
    xcb_gcontext_t gc = xcb_generate_id(window->connection);

    xcb_alloc_color_reply_t* bg = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            label->bg_color.r << 8,
            label->bg_color.g << 8,
            label->bg_color.b << 8
        ),
        NULL
    );
    xcb_alloc_color_reply_t* tx = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            label->text_color.r << 8,
            label->text_color.g << 8,
            label->text_color.b << 8
        ),
        NULL
    );

    uint32_t maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    uint32_t maskv[3] = {
        tx->pixel,
        bg->pixel,
        window->main_font
    };

    xcb_create_gc(
        window->connection,
        gc,
        window->window,
        maskd,
        maskv
    );

    int linecount = 0;

    int len = strlen(label->text);
    int c = 0;
    int d = 0;

    for (int i = 0; i < len; i++) {
        if (label->text[i] == '\n') {
            xcb_image_text_8(
                window->connection, 
                c,
                window->window,
                gc,
                widget->x + 3,
                widget->y + 17 + 20 * linecount,
                label->text + d - c
            );
            linecount++;
            c = 0;
        } else {
            c++;
        }
        d++;
    }

    xcb_image_text_8(
        window->connection, 
        c,
        window->window,
        gc,
        widget->x + 3,
        widget->y + 17 + 20 * linecount,
        label->text + d - c
    );
    xcb_free_gc(window->connection, gc);
    
    xcb_rectangle_t *rect = malloc(sizeof(xcb_rectangle_t));
    
    rect->x = widget->x;
    rect->y = widget->y;
    rect->width = widget->width;
    rect->height = widget->height;
    
    xcb_point_t points[2];
    
    // top
    if (!(widget->style & STYLE_NBT)) {
        points[0].x = widget->x;
        points[0].y = widget->y;
        points[1].x = widget->x + widget->width;
        points[1].y = widget->y;
        
        xcb_poly_line(
            window->connection,
            CoordModeOrigin,
            window->window,
            window->gc,
            2, &points
        );
    }

    
    // right
    if (!(widget->style & STYLE_NBR)) {
        points[0].x = widget->x + widget->width;
        points[0].y = widget->y;
        points[1].x = widget->x + widget->width;
        points[1].y = widget->y + widget->height;

        xcb_poly_line(
            window->connection,
            CoordModeOrigin,
            window->window,
            window->gc,
            2, &points
        );
    }

    // bottom
    if (!(widget->style & STYLE_NBB)) {
        points[0].x = widget->x + widget->width;
        points[0].y = widget->y + widget->height;
        points[1].x = widget->x;
        points[1].y = widget->y + widget->height;
        
        xcb_poly_line(
            window->connection,
            CoordModeOrigin,
            window->window,
            window->gc,
            2, &points
        );
    }

    // left
    if (!(widget->style & STYLE_NBL)) {
        points[0].x = widget->x;
        points[0].y = widget->y + widget->height;
        points[1].x = widget->x;
        points[1].y = widget->y;
        
        xcb_poly_line(
            window->connection,
            CoordModeOrigin,
            window->window,
            window->gc,
            2, &points
        );
    }

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

void label_set_color(widget_t* widget, int type, rgba_t value) {
    label_t* label = widget->extra_data;
    
    if (type == LABEL_BG_COLOR) {
        label->bg_color = value;
    } else if (type == LABEL_TEXT_COLOR) {
        label->text_color = value;
    }
}
void label_free(widget_t* widget) {
    label_t* label = widget->extra_data;

    free(label->text);
    free(label);
    widget_free(widget);
}