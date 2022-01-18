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

#include <common/util.h>
#include <ui/widgets/frame.h>
#include <ui/widgets/button.h>
#include <ui/uitypes.h>

class menubar_t;
class menu_t;
class menubutton_t;

class menubar_t : public widget_t {
    private:
        static int button_clicked(button_t* button, int x, int y);

    public:
        menubar_t();
        
        void* operator new(size_t count);
        void  operator delete(void* address);

        frame_t* container;
        int      ow;
        int      next_menu_x;
        
        int      menu_count;
        menu_t** menus;

        void    draw();
        int     clicked(int x, int y);
        menu_t* add_menu(char* name);
};

class menu_t : public widget_t {
    private:
        static int button_clicked(button_t* button, int x, int y);

    public:
        menu_t();
    
        void* operator new(size_t count);
        void  operator delete(void* address);

        button_t*      open_button;
        frame_t*       container;
        
        bool           is_open;
        int            next_button_y;
        
        int            button_count;
        menubutton_t** buttons;

        int           clicked(int x, int y);
        menubutton_t* add_button(char* name, void (*clicked)());
};

class menubutton_t : public widget_t {
    public:
        menubutton_t();

        void* operator new(size_t count);
        void  operator delete(void* address);
        
        button_t* button;

        void (*on_click)();
};

widget_t*     menubar_init();
menu_t*       menubar_add_menu(widget_t*, char*);
menubutton_t* menu_add_button(menu_t*, char*, void(*clicked)());
void          menubar_free(widget_t*);