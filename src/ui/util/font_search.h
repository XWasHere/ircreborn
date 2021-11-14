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

#ifndef IRCREBORN_FONT_SEARCH_H
#define IRCREBORN_FONT_SEARCH_H

#ifndef WIN32
#include <xcb/xcb.h>

#define FSEARCH_STYLE_NONE 0

#define FSEARCH_SLANT_UPRIGHT 0

#define FSEARCH_SPACING_MONO 0

typedef struct font_request font_request_t;
struct font_request {
    int want_slant;
    int want_style;
    int want_resx;
    int want_resy;
    int want_spacing;
    int slant;
    int style;
    int resx;
    int resy;
    int spacing;
};

xcb_font_t request_font(xcb_connection_t* connection, font_request_t* req);

#endif
#endif