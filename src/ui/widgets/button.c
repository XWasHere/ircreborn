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

#include <windows.h>

#include <ui/widgets/button.h>
#include <ui/widget.h>
#include <common/util.h>

void button_draw(widget_t*, window_t*);
void button_clicked(widget_t*, window_t*, int, int);
void button_mousein(widget_t*, window_t*);
void button_mouseout(widget_t*, window_t*);

widget_t* button_init() {
    widget_t* widget = widget_init();
    button_t* button = malloc(sizeof(button_t));

    widget->extra_data = button;

    widget->draw    = &button_draw;
    widget->clicked = &button_clicked;
    widget->mousein = &button_mousein;
    widget->mouseout= &button_mouseout;

    button->bg_color     = GetSysColor(COLOR_BTNFACE);
    button->text_color   = GetSysColor(COLOR_BTNTEXT);
    button->border_color = GetSysColor(COLOR_BTNFACE);

    button->widget = widget;

    return button->widget;
}

void button_set_type(widget_t* widget, int type) {
    button_t* btn = widget->extra_data;
    btn->type = type;
}

void button_draw(widget_t* widget, window_t* window) {
    button_t* button = widget->extra_data;

    // ALL THIS TO DRAW A FUCKING RECTANGLE
    PAINTSTRUCT* hi = malloc(sizeof(PAINTSTRUCT));
    RECT *rect = malloc(sizeof(RECT));

    SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
    
    // need this so beginpaint doesnt obliterate everything else trol
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    SelectObject   (hi->hdc, GetStockObject(DC_BRUSH));
    SelectObject   (hi->hdc, GetStockObject(DC_PEN));
    
    // i couldnt figure out a good way to style buttons so ill just do this (sorry)
    SetDCBrushColor(hi->hdc, button->bg_color);
    SetDCPenColor  (hi->hdc, button->border_color);
    SetBkColor     (hi->hdc, button->bg_color);
    SetTextColor   (hi->hdc, button->text_color);

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);

    if (button->type == BUTTON_TEXT) {
        DrawText(hi->hdc, button->text, strlen(button->text), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
}

void button_clicked(widget_t* widget, window_t* window, int x, int y) {
    // just here in case a dev doesnt implement the clicked thing    
}

void button_mousein(widget_t* widget, window_t* window) {
    button_draw(widget, window);
}

void button_mouseout(widget_t* widget, window_t* window) {

}
