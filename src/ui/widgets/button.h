#ifndef LIBUI_WIDGETS_BUTTON_H
#define LIBUT_WIDGETS_BUTTON_H

#include <ui/widget.h>
#include <windows.h> // agony

#define BUTTON_NULL 0x00
#define BUTTON_TEXT 0x01

typedef struct __button button_t;
struct __button {
    widget_t* widget;
    
    int type;

    char* text;

    COLORREF bg_color;
    COLORREF text_color;
    COLORREF border_color;
};

widget_t* button_init();
void      button_set_type(widget_t* widget, int type);

#endif
