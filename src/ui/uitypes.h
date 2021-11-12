/*
    due to includes breaking whenever these types were used this has to be kept seperate.

    i have no idea how this happened.
*/

#ifndef IRCREBORN_UITYPES_H
#define IRCREBORN_UITYPES_H

#include <windows.h>

typedef struct __widget widget_t;
typedef struct __window window_t;

struct __widget {
    // position
    int x;
    int y;

    // size
    int width;
    int height;

    // for widgets
    void* extra_data;

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
    void (*keypress)(widget_t* widget, window_t* window, char key);
};

// this is a window.
// TODO: xorg support
struct __window {
    // windows api bullshit
    HWND       window;
    WNDCLASS   window_class;
    ATOM       class_thing;
    HINSTANCE  instance;
    RECT       client_rect;

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