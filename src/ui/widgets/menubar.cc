/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2022 IRCReborn Devs

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

#include <ui/widgets/menubar.h>
#include <ui/widgets/button.h>
#include <string.h>
#include <stdlib.h>

void menubar_t::draw() {
    this->container->draw();

    for (int i = 0; i < this->menu_count; i++) {
        if (this->menus[i]->is_open) {
            this->menus[i]->container->draw();
        }
    }
}

int menubar_t::clicked(int x, int y) {
    this->window->paint();

    if (y < 20) {
        for (int i = 0; i < this->menu_count; i++) this->menus[i]->is_open = 0;
        this->container->clicked(x, y);
        return 1;
    } else {   
        for (int i = 0; i < this->menu_count; i++) {
            menu_t* menu = this->menus[i];
            if (menu->is_open) {
                menu->container->clicked(x, y);
                for (int i = 0; i < this->menu_count; i++) this->menus[i]->is_open = 0;
                return 1;
            }
        }
        return 0;
    }
}

int menu_t::clicked(int x, int y) {
    this->is_open = 0;
    return 1;
}

menubar_t::menubar_t() {
    this->container  = new frame_t();
    this->menu_count = 0;
    this->menus      = (menu_t**)malloc(1);
    this->next_menu_x= 0;
}

menubar_t::~menubar_t() {
    this->container->item_count = 0;

    for (int i = 0; i < this->menu_count; i++) {
        free(this->container->items[i]);
        delete this->menus[i];
    }

    free(this->menus);
    delete this->container;
}

menubutton_t::~menubutton_t() {
    delete this->button;
}

menu_t::~menu_t() {
    this->container->item_count = 0;

    for (int i = 0; i < this->button_count; i++) {
        free(this->container->items[i]);
        delete this->buttons[i];
    }

    delete this->open_button;
    delete this->container;
    
    free(this->buttons);
}

void* menubar_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void menubar_t::window_set(window_t* window) {
    this->window = window;
    this->container->window = window;
    
    for (int i = 0; i < this->menu_count; i++) {
        this->menus[i]->window = window;
        this->menus[i]->container->window = window;
        this->menus[i]->open_button->window = window;

        for (int ii = 0; ii < this->menus[i]->button_count; ii++) {
            this->menus[i]->buttons[ii]->window = window;
            this->menus[i]->buttons[ii]->button->window = window;
        }
    }
}

void menubar_t::operator delete(void* address) {
    free(address);
}

int menubar_t::button_clicked(button_t* button, int x, int y) {
    menu_t*       menu       = (menu_t*)button->parent;
    menubar_t*    menubar    = (menubar_t*)menu->parent;

    menu->is_open = 1;
    menu->container->draw();
    
    // this stupid hack makes sure that the menu closes if the user
    // clicks elsewhere
    menubar->height = 10000;
    
    return 1;
}

int menu_t::button_clicked(button_t* button, int x, int y) {
    menubutton_t* btn = (menubutton_t*)button->parent;

    btn->on_click();

    return 1;
}

menu_t::menu_t() {
    this->next_button_y = 0;
    this->open_button   = new button_t();
    this->button_count  = 0;
    this->container     = new frame_t();
    this->is_open       = 0;
    this->buttons       = (menubutton_t**)malloc(1);
    
    this->open_button->on_clicked = menubar_t::button_clicked;
    this->open_button->parent = this;
}

void* menu_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void menu_t::operator delete(void* address) {
    free(address);
}

menu_t* menubar_t::add_menu(char* name) {
    menu_t* menu = new menu_t();
        
    this->menu_count++;
    this->menus = (menu_t**)realloc(this->menus, sizeof(void*) * this->menu_count);
    this->menus[this->menu_count - 1] = menu;
    
    menu->open_button->set_text(name);
    menu->open_button->type = BUTTON_TEXT;

    menu->open_button->width = strlen(menu->open_button->text) * 10;
    menu->open_button->height = 20;

    frame_managed_t* item = this->container->add_item(menu->open_button);
    item->x = this->next_menu_x;
    item->y = 0;

    this->next_menu_x += menu->open_button->width;

    menu->container->y = 20;
    menu->container->x = item->x;
    menu->container->window = this->window;
    menu->open_button->window = this->window;
    menu->parent = this;

    return menu;
}

menubutton_t::menubutton_t() {
    this->button = new button_t();
    this->button->on_clicked = menu_t::button_clicked;
    this->button->height = 20;
    this->button->type = BUTTON_TEXT;
}

void* menubutton_t::operator new(size_t count) {
    void* t = malloc(count);
    memset(t, 0, count);
    return t;
}

void menubutton_t::operator delete(void* address) {
    free(address);
}

menubutton_t* menu_t::add_button(char* name, void(*clicked)()) {
    this->button_count++;
    this->buttons = (menubutton_t**)realloc(this->buttons, sizeof(void*) * this->button_count);
    
    menubutton_t* button = new menubutton_t();
    button->button->width = strlen(name) * 10;
    button->button->parent = button;
    button->button->set_text(name);
    button->parent = this;
    button->on_click = clicked;

    frame_managed_t* item = this->container->add_item(button->button);
    item->x = 0;
    item->y = this->next_button_y;

    this->next_button_y += 20;
    this->buttons[this->button_count-1] = button;

    return button;
}