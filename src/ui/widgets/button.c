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

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#else
#include <xcb/xcb.h>
#endif

#include <ui/widgets/button.h>
#include <ui/widget.h>
#include <ui/util/font_search.h>

#include <common/color.h>
#include <common/util.h>

void button_draw(widget_t*, window_t*);

widget_t* button_init() {
    widget_t* widget = widget_init();
    button_t* button = malloc(sizeof(button_t));

    widget->extra_data = button;

    widget->draw    = &button_draw;

#ifdef WIN32
    button->bg_color     = GetSysColor(COLOR_BTNFACE);
    button->text_color   = GetSysColor(COLOR_BTNTEXT);
    button->border_color = GetSysColor(COLOR_BTNFACE);
#else
    
#endif

    button->widget = widget;

    return widget;
}

void button_set_type(widget_t* widget, int type) {
    button_t* btn = widget->extra_data;
    btn->type = type;
}

void button_draw(widget_t* widget, window_t* window) {
    button_t* button = widget->extra_data;
#ifdef WIN32
    // ALL THIS TO DRAW A FUCKING RECTANGLE
    PAINTSTRUCT* hi = malloc(sizeof(PAINTSTRUCT));
    RECT *rect = malloc(sizeof(RECT));

    SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
    
    // need this so beginpaint doesnt obliterate everything else trol
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    SelectObject   (hi->hdc, GetStockObject(DC_BRUSH));
    SelectObject   (hi->hdc, GetStockObject(DC_PEN));
    
    // i couldnt figure out a good way to style buttons so ill just do this (sorry)
    SetDCBrushColor(hi->hdc, button->bg_color);
    SetDCPenColor  (hi->hdc, button->border_color);
    SetBkColor     (hi->hdc, button->bg_color);
    SetTextColor   (hi->hdc, button->text_color);

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    if (button->type == BUTTON_TEXT) {
        DrawText(hi->hdc, button->text, strlen(button->text), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
#else
    // get a rectangle
    xcb_rectangle_t* rect = malloc(sizeof(xcb_rectangle_t));
    
    // ???
    rect->x = widget->x;
    rect->y = widget->y;
    rect->width = widget->width;
    rect->height = widget->height;

    xcb_alloc_color_cookie_t c = xcb_alloc_color(
        window->connection,
        window->cmap,
        button->bg_color.r << 8,
        button->bg_color.g << 8,
        button->bg_color.b << 8
    );

    xcb_alloc_color_reply_t* bg = xcb_alloc_color_reply(
        window->connection,
        c,
        NULL
    );
    
    c = xcb_alloc_color(
        window->connection,
        window->cmap,
        button->text_color.r << 8,
        button->text_color.g << 8,
        button->text_color.b << 8
    );

    xcb_alloc_color_reply_t* tx = xcb_alloc_color_reply(
        window->connection,
        c,
        NULL
    );

    c =  xcb_alloc_color(
        window->connection,
        window->cmap,
        button->border_color.r << 8,
        button->border_color.g << 8,
        button->border_color.b << 8
    );

    xcb_alloc_color_reply_t* bd = xcb_alloc_color_reply(
        window->connection,
        c,
        NULL
    );

    uint32_t maskd = XCB_GC_FOREGROUND;
    uint32_t maskv[3] = {
        bg->pixel
    };

    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    if (button->type != BUTTON_INVIS) {
        xcb_poly_fill_rectangle(
            window->connection,
            window->window,
            window->gc,
            1,
            rect
        );
    }

    if (button->type == BUTTON_TEXT) {
        maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
        maskv[0] = tx->pixel;
        maskv[1] = bg->pixel;
        maskv[2] = window->main_font;

        xcb_change_gc(
            window->connection,
            window->gc,
            maskd,
            maskv
        );

        xcb_image_text_8(
            window->connection,
            strlen(button->text),
            window->window,
            window->gc,
            widget->x + 3,
            widget->y + widget->height - 3,
            button->text
        );
    }

    maskd = XCB_GC_FOREGROUND;
    maskv[0] = bd->pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    if (button->type != BUTTON_INVIS) {
        xcb_poly_rectangle(
            window->connection,
            window->window,
            window->gc,
            1,
            rect
        );
    }

    xcb_flush(window->connection);
    
    // bye
    free(rect);
#endif
}

// this exists because it gets really chaotic setting these values in client.c
void button_set_color(widget_t* widget, int t, rgba_t color) {
    button_t* button = widget->extra_data;

    if (t == BUTTON_COLOR_BG) {
        button->bg_color = color;
    } else if (t == BUTTON_COLOR_BR) {
        button->border_color = color;
    } else if (t == BUTTON_COLOR_TX) {
        button->text_color = color;
    }
}

void button_set_text(widget_t* widget, char* text) {
    button_t* button = widget->extra_data;
    
    char* buf = malloc(strlen(text) + 1);
    memset(buf, 0, strlen(text) + 1);
    strcpy(buf, text);
    button->text = buf;
}

void button_free(widget_t* widget) {
    button_t* button = widget->extra_data;

    widget_free(widget);
    free(button->text);
    free(button);
}