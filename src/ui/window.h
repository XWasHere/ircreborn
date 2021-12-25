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

#ifndef IRCREBORN_UI_WINDOW_H
#define IRCREBORN_UI_WINDOW_H

#include <ui/uitypes.h>

#define   WINDOW_WM_TYPE_DIALOG 1

window_t* window_init      ();
void      window_display   (window_t* window, int all);
void      window_add_widget(window_t* window, widget_t* widget);
void      window_set_focus (window_t* window, widget_t* widget);
void      window_paint     (window_t* window);
void      window_set_size  (window_t* window, int width, int height);
void      window_set_type  (window_t* window, int type);
void      window_set_bg    (window_t* window, rgba_t value);
void      window_free      (window_t* window);

#endif
