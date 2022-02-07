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

#include <ui/uitypes.h>
#include <ui/window.h>
#include <ui/widgets/label.h>
#ifndef WIN32 
#include <ui/util/font_search.h>
#endif
#include <stdlib.h>
#include <string.h>

void label_t::draw() {
#ifdef WIN32
    PAINTSTRUCT* hi = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
    RECT *rect = (RECT*)malloc(sizeof(RECT));
    
    SetRect(rect, this->x, this->y, this->x + this->width, this->y + this->height);
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    SelectObject(hi->hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hi->hdc, W32RGBAC(this->bg_color));
    SetBkColor(hi->hdc, W32RGBAC(this->bg_color));
    SetTextColor(hi->hdc, W32RGBAC(this->text_color));
    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);
    
    SetRect(rect, this->x + 1, this->y + 1, this->x + this->width - 1, this->y + this->height - 1);
    DrawText(hi->hdc, this->text, strlen(this->text), rect, 0);

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
#else
    // literally copied from button.c
    xcb_gcontext_t gc = xcb_generate_id(this->window->connection);

    xcb_alloc_color_reply_t* bg = xcb_alloc_color_reply(
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
    xcb_alloc_color_reply_t* tx = xcb_alloc_color_reply(
        this->window->connection,
        xcb_alloc_color(
            this->window->connection,
            this->window->cmap,
            this->text_color.r << 8,
            this->text_color.g << 8,
            this->text_color.b << 8
        ),
        NULL
    );

    uint32_t maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    uint32_t maskv[3] = {
        tx->pixel,
        bg->pixel,
        this->window->main_font
    };

    xcb_create_gc(
        this->window->connection,
        gc,
        this->window->window,
        maskd,
        maskv
    );

    int linecount = 0;

    int len = strlen(this->text);
    int c = 0;
    int d = 0;

    int w = 0;

    xcb_query_font_reply_t* qfr;
    xcb_charinfo_t* cinfo = xcb_query_font_char_infos(
        qfr = xcb_query_font_reply(
            this->window->connection,
            xcb_query_font_unchecked(
                this->window->connection,
                this->window->main_font
            ),
            NULL
        )
    );

    for (int i = 0; i < len; i++) {
        if (this->text[i] == '\n') {
            xcb_image_text_8(
                this->window->connection, 
                c,
                this->window->window,
                gc,
                this->x + 3,
                this->y + 17 + 20 * linecount,
                this->text + d - c
            );
            linecount++;
            c = 0;
            w = 0;
        } else if (w > this->width) {
            xcb_image_text_8(
                this->window->connection, 
                c,
                this->window->window,
                gc,
                this->x + 3,
                this->y + 17 + 20 * linecount,
                this->text + d - c
            );
            linecount++;
            c = 0;
            w = 0;
            w += cinfo[this->text[i]].character_width;
            c++;
        } else {
            c++;
            w += cinfo[this->text[i]].character_width;
        }
        d++;
    }

    free(qfr);

    xcb_image_text_8(
        this->window->connection, 
        c,
        this->window->window,
        gc,
        this->x + 3,
        this->y + 17 + 20 * linecount,
        this->text + d - c
    );
    xcb_free_gc(this->window->connection, gc);
    
    xcb_rectangle_t *rect = (xcb_rectangle_t*)malloc(sizeof(xcb_rectangle_t));
    
    rect->x = this->x;
    rect->y = this->y;
    rect->width = this->width;
    rect->height = this->height;
    
    xcb_point_t points[2];
    
    // top
    if (!(this->style & STYLE_NBT)) {
        points[0].x = this->x;
        points[0].y = this->y;
        points[1].x = this->x + this->width;
        points[1].y = this->y;
        
        xcb_poly_line(
            window->connection,
            CoordModeOrigin,
            window->window,
            window->gc,
            2, (xcb_point_t*)&points
        );
    }

    
    // right
    if (!(this->style & STYLE_NBR)) {
        points[0].x = this->x + this->width;
        points[0].y = this->y;
        points[1].x = this->x + this->width;
        points[1].y = this->y + this->height;

        xcb_poly_line(
            this->window->connection,
            CoordModeOrigin,
            this->window->window,
            this->window->gc,
            2, (xcb_point_t*)&points
        );
    }

    // bottom
    if (!(this->style & STYLE_NBB)) {
        points[0].x = this->x + this->width;
        points[0].y = this->y + this->height;
        points[1].x = this->x;
        points[1].y = this->y + this->height;
        
        xcb_poly_line(
            this->window->connection,
            CoordModeOrigin,
            this->window->window,
            this->window->gc,
            2, (xcb_point_t*)&points
        );
    }

    // left
    if (!(this->style & STYLE_NBL)) {
        points[0].x = this->x;
        points[0].y = this->y + this->height;
        points[1].x = this->x;
        points[1].y = this->y;
        
        xcb_poly_line(
            this->window->connection,
            CoordModeOrigin,
            this->window->window,
            this->window->gc,
            2, (xcb_point_t*)&points
        );
    }

    xcb_flush(this->window->connection);
    
    free(tx);
    free(bg);
    free(rect);
#endif
}

int label_t::calc_height() {
    int h = 20;

#ifdef linux
    int w = 0;

    xcb_query_font_reply_t* r = xcb_query_font_reply(
        this->window->connection,
        xcb_query_font_unchecked(
            this->window->connection,
            this->window->main_font
        ),
        NULL
    );

    for (int i = 0; this->text[i] != 0; i++) {
        w += xcb_query_font_char_infos(r)[this->text[i]].character_width;
        if (this->text[i] == '\n') {
            h += 20;
        } else if (w > this->width) {
            h += 20;
            w = 0;
        }
    }

    free(r);
#endif
    
    return h;
}

void label_t::set_text(char* text) {
    char* buf = (char*)malloc(strlen(text) + 1);

    memset(buf, 0, strlen(text) + 1);
    strcpy(buf, text);
    
    free(this->text);
    this->text = buf;
}

void* label_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void label_t::operator delete(void* address) {
    free(address);
}

label_t::label_t() {
    this->text    = malloc(1);
    this->text[0] = 0;
}

label_t::~label_t() {
    free(this->text);
}