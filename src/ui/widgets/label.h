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

#ifndef IRCREBORN_UI_LABEL_H
#define IRCREBORN_UI_LABEL_H

#define LABEL_BG_COLOR 0x00
#define LABEL_TEXT_COLOR 0x01

#include <ui/window.h>
#include <ui/widget.h>

typedef struct __label label_t;
struct __label {
    widget_t* widget;

    char* text;

    rgba_t bg_color;
    rgba_t text_color;
};

widget_t* label_init();
void      label_set_text(widget_t* widget, char* text);
void      label_set_color(widget_t* widget, int type, rgba_t value);
void      label_free(widget_t* widget);

#endif