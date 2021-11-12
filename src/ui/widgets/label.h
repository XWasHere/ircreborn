#ifndef IRCREBORN_UI_LABEL_H
#define IRCREBORN_UI_LABEL_H

#include <ui/window.h>
#include <ui/widget.h>

typedef struct __label label_t;
struct __label {
    widget_t* widget;

    char* text;
    int   len;
};

widget_t* label_init();

#endif