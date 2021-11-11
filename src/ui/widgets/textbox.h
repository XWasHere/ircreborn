#ifndef IRCREBORN_UI_TEXTBOX_H
#define IRCREBORN_UI_TEXTBOX_H

#include <ui/widget.h>

typedef struct __textbox textbox_t;
struct __textbox {
    widget_t* widget;
    
    int multiline;

    char* text;
    
    int textlen;
    int cursorpos;

    void (*submit)(widget_t* widget, void* window, char* text, int textlen);
};

widget_t* textbox_init();

#endif