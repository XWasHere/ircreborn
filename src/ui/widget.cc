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
#include <stdlib.h>

void widget_t::draw() {}
int  widget_t::clicked(int c, int d) { return 1; }
int  widget_t::mousein() { return 1; }
int  widget_t::mouseout() { return 1; }
int  widget_t::mousedown(int c, int d) { return 1; }
int  widget_t::mouseup(int c, int d) { return 1; }
int  widget_t::mousemove(int c, int d) { return 1; }
int  widget_t::keypress(uint32_t c, uint16_t s) { return 1; }
int  widget_t::scroll_up() { return 1; }
int  widget_t::scroll_down() { return 1; }
void widget_t::window_set(window_t* w) { this->window = w; }

void* widget_t::operator new(size_t count) {
    void* ptr = malloc(count);
    memset(ptr, 0, count);
    return ptr;
}

widget_t::widget_t() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->width = 0;
}

widget_t::~widget_t() {
    
}