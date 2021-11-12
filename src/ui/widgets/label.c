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

#include <ui/widget.h>
#include <ui/window.h>
#include <ui/widgets/label.h>

void label_draw(widget_t* widget ,window_t* window) {
    label_t* label = widget->extra_data;
    
    PAINTSTRUCT* hi = malloc(sizeof(PAINTSTRUCT));
    RECT *rect = malloc(sizeof(RECT));
    
    SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);
    
    SetRect(rect, widget->x + 1, widget->y + 1, widget->x + widget->width - 1, widget->y + widget->height - 1);
    DrawText(hi->hdc, label->text, strlen(label->text), rect, 0);

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
}

widget_t* label_init() {
    widget_t* label = widget_init();
    label_t*  lab   = malloc(sizeof(label_t));

    lab->len  = 0;
    lab->text = 0;

    lab->widget = label;
    label->extra_data = lab;

    label->draw = &label_draw;
    
    return label;
}