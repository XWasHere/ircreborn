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

#include <ui/window.h>
#include <ui/widget.h>
#include <stdlib.h>

void __DEFAULT_draw(widget_t* a, window_t* b) {}
int  __DEFAULT_clicked(widget_t* a, window_t* b, int c, int d) { return 1; }
int  __DEFAULT_mousein(widget_t* a, window_t* b) { return 1; }
int  __DEFAULT_mouseout(widget_t* a, window_t* b) { return 1; }
int  __DEFAULT_mousedown(widget_t* a, window_t* b, int c, int d) { return 1; }
int  __DEFAULT_mouseup(widget_t* a, window_t* b, int c, int d) { return 1; }
int  __DEFAULT_mousemove(widget_t* a, window_t* b, int c, int d) { return 1; }
int  __DEFAULT_keypress(widget_t* a, window_t* b, uint32_t c, uint16_t s) { return 1; }
int  __DEFAULT_scroll_up(widget_t* a, window_t* b) { return 1; }
int  __DEFAULT_scroll_down(widget_t* a, window_t* b) { return 1; }

widget_t* widget_init() {
    widget_t* widget = (widget_t*)malloc(sizeof(widget_t));

    widget->draw = &__DEFAULT_draw;
    widget->clicked = &__DEFAULT_clicked;
    widget->mousein = &__DEFAULT_mousein;
    widget->mouseout = &__DEFAULT_mouseout;
    widget->mousedown = &__DEFAULT_mousedown;
    widget->mouseup = &__DEFAULT_mouseup;
    widget->mousemove = &__DEFAULT_mousemove;
    widget->keypress = &__DEFAULT_keypress;
    widget->scroll_down = &__DEFAULT_scroll_down;
    widget->scroll_up = &__DEFAULT_scroll_up;
    widget->x = 0;
    widget->y = 0;
    widget->z = 0;
    widget->style = 0;

    return widget;
}

void widget_free(widget_t* widget) {
    free(widget);
}