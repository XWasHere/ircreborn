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

#ifndef IRCREBORN_UI_FRAME_H
#define IRCREBORN_UI_FRAME_H

#include <ui/widget.h>

#define FRAME_COLOR_BG 0x00

typedef struct __frame         frame_t;
typedef struct __frame_managed frame_managed_t;

struct __frame {
    widget_t* widget;

    rgba_t            bg_color;
    int               item_count;
    frame_managed_t** items;
};

struct __frame_managed {
    widget_t* widget;

    int x;
    int y;
};

widget_t*        frame_init();
frame_managed_t* frame_add_item(frame_t* frame, widget_t* widget);
void             frame_set_color(widget_t* widget, int type, rgba_t value);
void             frame_free(widget_t* widget);

#endif