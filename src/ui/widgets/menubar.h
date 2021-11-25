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

#include <ui/widgets/frame.h>
#include <ui/uitypes.h>

typedef struct __menubar    menubar_t;
typedef struct __menu       menu_t;
typedef struct __menubutton menubutton_t;

struct __menubar {
    widget_t* widget;
    widget_t* container;
    int       ow;
    int       next_menu_x;
    int       menu_count;
    menu_t**  menus;
};

struct __menu {
    widget_t*      widget;
    widget_t*      open_button;
    widget_t*      container;
    int            is_open;
    int            next_button_y;
    int            button_count;
    menubutton_t** buttons;
};

struct __menubutton {
    widget_t* widget;
    widget_t* button;
    void (*clicked)();
};

widget_t*     menubar_init();
menu_t*       menubar_add_menu(widget_t*, char*);
menubutton_t* menu_add_button(menu_t*, char*, void(*clicked)());
void          menubar_free();