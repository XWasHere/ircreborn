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

#include <ui/uitypes.h>

#define SCROLLPANE_COLOR_BG     0x00
#define SCROLLPANE_COLOR_THUMB  0x01
#define SCROLLPANE_COLOR_TRACK  0x02
#define SCROLLPANE_COLOR_BUTTON 0x03

class scroll_pane_t;
class scroll_pane_item_t;

class scroll_pane_item_t : public widget_t {
    public:
        widget_t*           widget;

        int                 x;
        int                 y;
};

struct scroll_pane_t : public widget_t {
    public:
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

        scroll_pane_t();
        
        void* operator new(size_t count);
        void  operator delete(void* address);
        
        scroll_pane_item_t* add_item(widget_t* widget);
        
        void draw();
        int clicked(int x, int y);
        int mousedown(int x, int y);
        int mouseup(int x, int y);
        int mouseout();
        int mousemove(int x, int y);
        int scroll_up();
        int scroll_down();
};

#endif
