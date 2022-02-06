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
#include <string.h>

#include <ui/widgets/textbox.h>
#include <ui/uitypes.h>
#include <ui/window.h>

#ifdef WIN32
#include <windows.h>
#else
#include <xcb/xcb.h>
#include <ui/util/font_search.h>
#endif

void textbox_t::default_submit(textbox_t* tb, char* c, int d) {};

int textbox_t::keypress(uint32_t key, uint16_t mod) {
    unsigned char K = key & 0xff; // for some reason it comes out as 0xFF(key)

    if (key == 0) {
        return 1;
    }
    
#ifdef WIN32
    if (key == 8) {
#else
    if (key == 0xff08 || key == 0x8) {
#endif
        if (this->cursorpos == 0) return 1;
        this->textlen--;
        this->cursorpos--;
        
        this->text[this->cursorpos] = 0;
    } else {
#ifdef WIN32
        if ((key == 10 || key == 13) && this->multiline == 0) {
#else
        if ((key == 0xd || key == 0xff0d) && (this->multiline == 2 || (this->multiline == 0 && !(mod&XCB_MOD_MASK_SHIFT)))) {
#endif
            this->submit(this, this->text, this->textlen);
            return 1;
        }

#ifndef WIN32
        if (key == 0xff0d || key==0xd) key = '\n';
        if (K == 0xe1 || K == 0xe2 || K == 0xe3 || K == 0xe4 || K == 0xe5 || K == 0xe6 || K == 0xe7 || K == 0xe8 || K == 0xe9 ) { return 1; }
#endif
        this->textlen++;

        if (this->text == 0) this->text = (char*)malloc(1);
        
        this->text = (char*)realloc(this->text, this->textlen + 1);
        
        this->text[this->cursorpos] = key;
        this->cursorpos++;
        this->text[this->cursorpos] = 0;
    }
    
    if (this->on_keypress) this->on_keypress(this, key, mod);

    this->draw();
    return 1;
}

int textbox_t::clicked(int x, int y) {
    window->focused = this;
    return 1;
}

void textbox_t::draw() {
#ifdef WIN32    
    PAINTSTRUCT* ps   = (PAINTSTRUCT*)malloc(sizeof(PAINTSTRUCT));
    RECT*        rect = (RECT*)malloc(sizeof(RECT));

    SetRect(rect, this->x, this->y, this->x + this->width, this->y + this->height);
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, ps);

    SelectObject(ps->hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(ps->hdc, W32RGBAC(this->bg_color));
    SetBkColor(ps->hdc, W32RGBAC(this->bg_color));
    SetTextColor(ps->hdc, W32RGBAC(this->text_color));

    int h = 0;

    Rectangle(ps->hdc, rect->left, rect->top, rect->right, rect->bottom);        

    if (this->text != 0) {
        SetRect(rect, this->x + 1, this->y + 1, this->x + this->width - 1, this->y + this->height - 1);
        DrawText(ps->hdc, this->text, strlen(this->text), rect, 0);
    }

    EndPaint(window->window, ps);

    free(ps);
    free(rect);
#else
    xcb_rectangle_t *rect = (xcb_rectangle_t*)malloc(sizeof(xcb_rectangle_t));
    
    rect->x = this->x;
    rect->y = this->y;
    rect->width = this->width;
    rect->height = this->height;
    
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

    uint32_t maskd = XCB_GC_FOREGROUND;
    uint32_t maskv[3] = {
        r->pixel
    };

    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    xcb_poly_fill_rectangle(
        window->connection,
        window->window,
        window->gc,
        1,
        rect
    );

    xcb_alloc_color_reply_t* tx = xcb_alloc_color_reply(
        window->connection,
        xcb_alloc_color(
            window->connection,
            window->cmap,
            this->text_color.r << 8,
            this->text_color.g << 8,
            this->text_color.b << 8
        ),
        NULL
    );
    maskd = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    maskv[0] = tx->pixel;
    maskv[1] = r->pixel;
    maskv[2] = window->main_font;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );

    int linecount = 0;

    int len = strlen(this->text);
    int c = 0;
    int d = 0;

    for (int i = 0; i < len; i++) {
        if (this->text[i] == '\n') {
            xcb_image_text_8(
                window->connection, 
                c,
                window->window,
                window->gc,
                this->x + 3,
                this->y + 17 + 20 * linecount,
                this->text + d - c
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
        window->gc,
        this->x + 3,
        this->y + 17 + 20 * linecount,
        this->text + d - c
    );
    
    maskv[0] = window->screen->black_pixel;
    xcb_change_gc(
        window->connection,
        window->gc,
        maskd,
        maskv
    );
    
    xcb_poly_rectangle(
        window->connection,
        window->window,
        window->gc,
        1,
        rect
    );

    xcb_flush(window->connection);
    
    // bye
    free(tx);
    free(r);
    free(rect);
#endif
}

textbox_t::textbox_t() {
    this->textlen = 0;
    this->cursorpos = 0;
    this->text = (char*)malloc(1);
    this->text[0] = 0;
    this->multiline = 0;
    this->submit = textbox_t::default_submit;
}

textbox_t::~textbox_t() {
    free(this->text);
}

void* textbox_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void textbox_t::operator delete(void* address) {
    free(address);
}