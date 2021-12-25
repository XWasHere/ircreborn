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

#include <common/color.h>

#define STYLE_NBT 0x1
#define STYLE_NBB 0x2
#define STYLE_NBR 0x4
#define STYLE_NBL 0x8

typedef struct __widget   widget_t;
typedef struct __window   window_t;

struct __widget {
    // position
    int x;
    int y;
    int z;

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

    // most widgets have borders, so im putting them here
    uint32_t style;

    // draw function prototype (pretend window is window_t*)
    void (*draw)(widget_t* widget, window_t* window);
    int  (*clicked)(widget_t* widget, window_t* window, int x, int y);
    int  (*mousein)(widget_t* widget, window_t* window);
    int  (*mouseout)(widget_t* widget, window_t* window);
    int  (*mousedown)(widget_t* widget, window_t* window, int x, int y);
    int  (*mouseup)(widget_t* widget, window_t* window, int x, int y);
    int  (*mousemove)(widget_t* widget, window_t* window, int x, int y);
    int  (*keypress)(widget_t* widget, window_t* window, uint32_t key, uint16_t mod);
    int  (*scroll_up)(widget_t* widget, window_t* window);
    int  (*scroll_down)(widget_t* widget, window_t* window);
};

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
    xcb_colormap_t    cmap;
    int               lshift_state;
    int               rshift_state;
    int               lctrl_state;
    int               rctrl_state;
#endif

    int               pending_free;

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

    rgba_t bg_color;
};

#endif