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
#include <ui/uitypes.h>
#include <ui/util/font_search.h>

#include <common/color.h>
#include <common/util.h>

void button_t::draw() {
#ifdef WIN32
    // ALL THIS TO DRAW A FUCKING RECTANGLE
    PAINTSTRUCT* hi = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
    RECT *rect = (RECT*)malloc(sizeof(RECT));

    SetRect(rect, this->x, this->y, this->x + this->width, this->y + this->height);
    
    // need this so beginpaint doesnt obliterate everything else trol
    InvalidateRect(this->window->window, rect, 1);
    BeginPaint(this->window->window, hi);

    SelectObject   (hi->hdc, GetStockObject(DC_BRUSH));
    SelectObject   (hi->hdc, GetStockObject(DC_PEN));
    
    // i couldnt figure out a good way to style buttons so ill just do this (sorry)
    SetDCBrushColor(hi->hdc, W32RGBAC(this->bg_color));
    SetDCPenColor  (hi->hdc, W32RGBAC(this->border_color));
    SetBkColor     (hi->hdc, W32RGBAC(this->bg_color));
    SetTextColor   (hi->hdc, W32RGBAC(this->text_color));

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    if (this->type == BUTTON_TEXT) {
        DrawText(hi->hdc, this->text, strlen(this->text), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    EndPaint(this->window->window, hi);
    
    free(hi);
    free(rect);
#else
    // get a rectangle
    xcb_rectangle_t* rect = (xcb_rectangle_t*)malloc(sizeof(xcb_rectangle_t));
    
    // ???
    rect->x = this->x;
    rect->y = this->y;
    rect->width = this->width;
    rect->height = this->height;

    xcb_alloc_color_cookie_t c = xcb_alloc_color(
        this->window->connection,
        this->window->cmap,
        this->bg_color.r << 8,
        this->bg_color.g << 8,
        this->bg_color.b << 8
    );

    xcb_alloc_color_reply_t* bg = xcb_alloc_color_reply(
        this->window->connection,
        c,
        NULL
    );
    
    c = xcb_alloc_color(
        this->window->connection,
        this->window->cmap,
        this->text_color.r << 8,
        this->text_color.g << 8,
        this->text_color.b << 8
    );

    xcb_alloc_color_reply_t* tx = xcb_alloc_color_reply(
        this->window->connection,
        c,
        NULL
    );

    c =  xcb_alloc_color(
        this->window->connection,
        this->window->cmap,
        this->border_color.r << 8,
        this->border_color.g << 8,
        this->border_color.b << 8
    );

    xcb_alloc_color_reply_t* bd = xcb_alloc_color_reply(
        this->window->connection,
        c,
        NULL
    );

    uint32_t maskd = XCB_GC_FOREGROUND;
    uint32_t maskv[3] = {
        bg->pixel
    };

    xcb_change_gc(
        this->window->connection,
        this->window->gc,
        maskd,
        maskv
    );

    if (this->type != BUTTON_INVIS) {
        xcb_poly_fill_rectangle(
            this->window->connection,
            this->window->window,
            this->window->gc,
            1,
            rect
        );
    }

    if (this->type == BUTTON_TEXT) {
        maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
        maskv[0] = tx->pixel;
        maskv[1] = bg->pixel;
        maskv[2] = this->window->main_font;

        xcb_change_gc(
            this->window->connection,
            this->window->gc,
            maskd,
            maskv
        );

        xcb_image_text_8(
            this->window->connection,
            strlen(this->text),
            this->window->window,
            this->window->gc,
            this->x + 3,
            this->y + this->height - 3,
            this->text
        );
    }

    maskd = XCB_GC_FOREGROUND;
    maskv[0] = bd->pixel;
    xcb_change_gc(
        this->window->connection,
        this->window->gc,
        maskd,
        maskv
    );

    if (this->type != BUTTON_INVIS) {
        xcb_poly_rectangle(
            this->window->connection,
            this->window->window,
            this->window->gc,
            1,
            rect
        );
    }

    xcb_flush(this->window->connection);
    
    // bye
    free(rect);
#endif
}

int button_t::clicked(int x, int y) {
    if (this->on_clicked) this->on_clicked(this, x, y);
}

// safely sets a button's text so it wont break when freeing it
void button_t::set_text(char* text) {
    char* buf = (char*)malloc(strlen(text) + 1);
    memset(buf, 0, strlen(text) + 1);
    strcpy(buf, text);
    this->text = buf;
    this->text_set = 1;
}

button_t::~button_t() {
    if (this->text && this->text_set) {
        free(this->text);
    }
}