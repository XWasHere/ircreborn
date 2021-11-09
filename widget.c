#include <ui/window.h>
#include <ui/widget.h>
#include <stdlib.h>

void __DEFAULT_draw(widget_t* a, window_t* b) {}
void __DEFAULT_clicked(widget_t* a, window_t* b, int c, int d) {}
void __DEFAULT_mousein(widget_t* a, window_t* b) {}
void __DEFAULT_mouseout(widget_t* a, window_t* b) {}
void __DEFAULT_mousedown(widget_t* a, window_t* b, int c, int d) {}
void __DEFAULT_mouseup(widget_t* a, window_t* b, int c, int d) {}
void __DEFAULT_mousemove(widget_t* a, window_t* b, int c, int d) {}

widget_t* widget_init() {
    widget_t* widget = malloc(sizeof(widget_t));

    widget->draw = &__DEFAULT_draw;
    widget->clicked = &__DEFAULT_clicked;
    widget->mousein = &__DEFAULT_mousein;
    widget->mouseout = &__DEFAULT_mouseout;
    widget->mousedown = &__DEFAULT_mousedown;
    widget->mouseup = &__DEFAULT_mouseup;
    widget->mousemove = &__DEFAULT_mousemove;
    
    return widget;
}