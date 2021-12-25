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

#ifndef IRCREBORN_UI_WIDGETS_BUTTON_H
#define IRCREBORN_UI_WIDGETS_BUTTON_H

#include <ui/widget.h>
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

typedef struct __button button_t;
struct __button {
    widget_t* widget;
    
    int type;

    char* text;

    rgba_t bg_color;
    rgba_t text_color;
    rgba_t border_color;
};

widget_t* button_init();
void      button_set_type(widget_t* widget, int type);
void      button_set_text(widget_t* widget, char* text);
void      button_set_color(widget_t* widget, int c, rgba_t color);
void      button_free(widget_t* widget);

#endif
