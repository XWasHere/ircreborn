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

#ifndef IRCREBORN_UI_WIDGET_SCROLLPANE_H
#define IRCREBORN_UI_WIDGET_SCROLLPANE_H

#include <ui/widget.h>

#define SCROLLPANE_COLOR_BG     0x00
#define SCROLLPANE_COLOR_THUMB  0x01
#define SCROLLPANE_COLOR_TRACK  0x02
#define SCROLLPANE_COLOR_BUTTON 0x03

typedef struct scroll_pane      scroll_pane_t;
typedef struct scroll_pane_item scroll_pane_item_t;

struct scroll_pane_item {
    widget_t*           widget;

    int                 x;
    int                 y;
};

struct scroll_pane {
    widget_t*            widget;

    scroll_pane_item_t** items;
    int                  itemc;

    int                  pos;
    int                  prev_pos;
    int                  csize;

    int                  thumb_dragging;
    int                  thumb_drag_src;
    int                  thumb_pos;

    rgba_t               bg_color;
    rgba_t               track_color;
    rgba_t               thumb_color;
    rgba_t               button_color;
};

widget_t* scroll_pane_init();
scroll_pane_item_t* scroll_pane_add_item(widget_t* scroll_pane, widget_t* widget);
void scroll_pane_set_color(widget_t* widget, int type, rgba_t value);
void scroll_pane_free(widget_t* widget);

#endif
