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

#ifndef IRCREBORN_UI_WIDGETS_BUTTON_H
#define IRCREBORN_UI_WIDGETS_BUTTON_H

#include <ui/uitypes.h>
#include <ui/window.h>

#ifdef WIN32
#include <windows.h> // agony
#else

#endif

#define BUTTON_NULL  0x00
#define BUTTON_TEXT  0x01
#define BUTTON_INVIS 0x02

#define BUTTON_COLOR_BG 0x00
#define BUTTON_COLOR_TX 0x01
#define BUTTON_COLOR_BR 0x02

class button_t : public widget_t {
    public:
        button_t();
        ~button_t();

        void* operator new(size_t count);
        void  operator delete(void* address);
        
        int type;

        char* text;
        int text_set;

        rgba_t bg_color;
        rgba_t text_color;
        rgba_t border_color;

        void set_text(char* text);
        void draw();
        int  clicked(int x, int y);

        int (*on_clicked)(button_t* button, int x, int y);
};

#endif
