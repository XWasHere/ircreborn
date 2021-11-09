#ifndef LIBUI_WIDGET_SCROLLPANE_H
#define LIBUI_WIDGET_SCROLLPANE_H

#include <ui/widget.h>

typedef struct scroll_pane      scroll_pane_t;
typedef struct scroll_pane_item scroll_pane_item_t;

struct scroll_pane_item {
    widget_t*           widget;

    int                 x;
    int                 y;
};

struct scroll_pane {
    widget_t*            widget;

    scroll_pane_item_t** items;
    int                  itemc;

    int                  pos;
    int                  csize;

    int                  thumb_dragging;
    int                  thumb_drag_src;
    int                  thumb_pos;
};

widget_t* scroll_pane_init();
scroll_pane_item_t* scroll_pane_add_item(widget_t* scroll_pane, widget_t* widget);

#endif