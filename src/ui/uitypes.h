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

typedef struct __widget   widget_t;
typedef struct __window   window_t;

struct __widget {
    // position
    int x;
    int y;

    // size
    int width;
    int height;

    // for widgets
    void* extra_data;
    void* extra_data_2;
    void* extra_data_3;
    void* extra_data_4;
    
    // for measuring the state in the window code
    int hovered;

    // draw function prototype (pretend window is window_t*)
    void (*draw)(widget_t* widget, window_t* window);
    void (*clicked)(widget_t* widget, window_t* window, int x, int y);
    void (*mousein)(widget_t* widget, window_t* window);
    void (*mouseout)(widget_t* widget, window_t* window);
    void (*mousedown)(widget_t* widget, window_t* window, int x, int y);
    void (*mouseup)(widget_t* widget, window_t* window, int x, int y);
    void (*mousemove)(widget_t* widget, window_t* window, int x, int y);
    void (*keypress)(widget_t* widget, window_t* window, uint32_t key);
};

// this is a window.
// TODO: xorg support
struct __window {
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
    int               lshift_state;
    int               rshift_state;
    int               lctrl_state;
    int               rctrl_state;
#endif

    // stuff
    int        width;
    int        height;

    // widgets
    widget_t** widgets; // cant use widget_t lmao
    int        widget_count;
    
    // keeping track of stuff
    int        mouse_left_down;
    widget_t*  focused;

    // do i even need to explain this.
    int        should_exit;

    void (*handle_bg_tasks)(window_t* window);
    void (*resized)(window_t* window);
};

#endif