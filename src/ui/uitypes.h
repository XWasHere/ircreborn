/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2022 IRCReborn Devs

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

/*
    due to includes breaking whenever these types were used this has to be kept seperate.

    i have no idea how this happened.
*/

#ifndef IRCREBORN_UITYPES_H
#define IRCREBORN_UITYPES_H

#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#else
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#endif

#include <common/color.h>
#include <common/util.h>

#define STYLE_NBT 0x1
#define STYLE_NBB 0x2
#define STYLE_NBR 0x4
#define STYLE_NBL 0x8

class window_t;
class widget_t;

class widget_t {
    public:
        window_t* window;
        widget_t* parent;

        int x;
        int y;
        int z;

        int width;
        int height;

        int hovered;

        widget_t();
        virtual ~widget_t();

        void* operator new(size_t count);
        void  operator delete(void* addr);

        virtual void draw();
        virtual int  clicked(int x, int y);
        virtual int  mousein();
        virtual int  mouseout();
        virtual int  mousedown(int x, int y);
        virtual int  mouseup(int x, int y);
        virtual int  mousemove(int x, int y);
        virtual int  keypress(uint32_t key, uint16_t mod);
        virtual int  scroll_up();
        virtual int  scroll_down();
        virtual void window_set(window_t* window);
};

// death of all good things
class window_t {
    private:
        int pending_free;

        // do i even need to explain this.
        int        should_exit;
        
    public:
#ifdef WIN32
        // windows api bullshit
        HWND       window;
        WNDCLASS   window_class;
        ATOM       class_thing;
        HINSTANCE  instance;
        RECT       client_rect;
#else
        Display*          display;
        xcb_connection_t* connection;
        xcb_window_t      window;
        xcb_gcontext_t    gc;
        xcb_screen_t*     screen;
        xcb_font_t        main_font;
        xcb_colormap_t    cmap;
        int               lshift_state;
        int               rshift_state;
        int               lctrl_state;
        int               rctrl_state;
#endif
        // stuff
        int        width;
        int        height;

        // widgets
        widget_t** widgets;
        int        widget_count;
        
        // keeping track of stuff
        int        mouse_left_down;
        widget_t*  focused;

        void (*handle_bg_tasks)(window_t* window);
        void (*on_resized)(window_t* window);

        rgba_t bg_color;

        static window_t** windows;
        static int window_count;
        static int window_count_2;

        window_t();
        ~window_t();
        void set_type(int type);
        void set_size(int height, int width);
        void keypress(uint32_t key, uint16_t mod);
        void run_bg_tasks();
        widget_t** sort_z();
        void close();
        void paint();
        void left_mouse_down(int x, int y);
        void left_mouse_up(int x, int y);
        void scroll_up(int x, int y);
        void scroll_down(int x, int y);
        void mouse_move(int x, int y);
        void resized(int x, int y);
        void show(int all);
        void add_widget(widget_t* widget);
        void remove_widget(widget_t* widget);

#ifdef WIN32
        static window_t* resolve_window(HWND window);
#endif
};

#endif