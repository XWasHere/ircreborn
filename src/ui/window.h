#ifndef IRCREBORN_UI_WINDOW_H
#define IRCREBORN_UI_WINDOW_H

#include <windows.h>

// this is a window.
// TODO: xorg support
typedef struct __window window_t;
struct __window {
    // windows api bullshit
    HWND       window;
    WNDCLASS   window_class;
    ATOM       class_thing;
    HINSTANCE  instance;
    RECT       client_rect;

    // widgets
    void**     widgets; // cant use widget_t lmao
    int        widget_count;
    
    // keeping track of stuff
    int        mouse_left_down;
    void*      focused;

    // do i even need to explain this.
    int        should_exit;

    void (*handle_bg_tasks)(window_t* window);
};

window_t* window_init      ();
void      window_display   (window_t* window);
void      window_add_widget(window_t* window, void* widget);
void      window_set_focus (window_t* window, void* widget);
#endif
