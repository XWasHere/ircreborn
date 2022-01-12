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

#include <ui/uitypes.h>

#define FRAME_COLOR_BG 0x00

typedef struct __frame_managed frame_managed_t;

class frame_t : public widget_t  {
    public:
        frame_t();
        ~frame_t();
        
        rgba_t            bg_color;
        int               item_count;
        frame_managed_t** items;

        void draw();
        int clicked(int x, int y);

        frame_managed_t* add_item(widget_t* widget);
};

struct __frame_managed {
    widget_t* widget;

    int x;
    int y;
};

#endif