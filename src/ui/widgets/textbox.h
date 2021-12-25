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

#ifndef IRCREBORN_UI_TEXTBOX_H
#define IRCREBORN_UI_TEXTBOX_H

#include <ui/widget.h>

#define TEXTBOX_COLOR_BG     0x00
#define TEXTBOX_COLOR_TEXT   0x01
#define TEXTBOX_COLOR_BORDER 0x02

typedef struct __textbox textbox_t;
struct __textbox {
    widget_t* widget;
    
    int multiline;

    char* text;
    
    int textlen;
    int cursorpos;

    void (*submit)(widget_t* widget, window_t* window, char* text, int textlen);

    rgba_t bg_color;
    rgba_t text_color;
    rgba_t border_color;
};

widget_t* textbox_init();
void      textbox_free(widget_t* widget);
void      textbox_set_color(widget_t* widget, int type, rgba_t value);

#endif