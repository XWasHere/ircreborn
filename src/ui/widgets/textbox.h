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

#ifndef IRCREBORN_UI_TEXTBOX_H
#define IRCREBORN_UI_TEXTBOX_H

#include <common/util.h>
#include <ui/uitypes.h>

#define TEXTBOX_COLOR_BG     0x00
#define TEXTBOX_COLOR_TEXT   0x01
#define TEXTBOX_COLOR_BORDER 0x02

class textbox_t : public widget_t {
    private:
        static void default_submit(textbox_t* tb, char* text, int textlen);

    public:
        textbox_t();
        ~textbox_t();
        
        int multiline;

        char* text;
        
        int textlen;
        int cursorpos;

        void (*submit)(textbox_t* tb, char* text, int textlen);
        void (*on_keypress)(textbox_t* tb, uint32_t key, uint16_t mod);

        rgba_t bg_color;
        rgba_t text_color;
        rgba_t border_color;

        void* operator new(size_t count);
        void  operator delete(void* address);
        
        int keypress(uint32_t key, uint16_t mod);
        int clicked(int x, int y);
        void draw();
        int calc_height();
};

widget_t* textbox_init();
void      textbox_free(widget_t* widget);
void      textbox_set_color(widget_t* widget, int type, rgba_t value);
int       textbox_keypress(widget_t* widget, window_t* window, uint32_t key, uint16_t mod);

#endif