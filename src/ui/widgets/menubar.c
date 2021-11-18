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

#include <ui/widgets/menubar.h>
#include <ui/widgets/button.h>

void menubar_draw(widget_t* widget, window_t* window) {
    menubar_t* menubar = widget->extra_data;

    menubar->container->draw(menubar->container, window);

    for (int i = 0; i < menubar->menu_count; i++) {
        if (menubar->menus[i]->is_open) {
            menubar->menus[i]->container->draw(menubar->menus[i]->container, window);
        }
    }
}

void menubar_clicked(widget_t* widget, window_t* window, int x, int y) {
    menubar_t* menubar = widget->extra_data;

    window_paint(window);

    if (y < 20) {
        for (int i = 0; i < menubar->menu_count; i++) menubar->menus[i]->is_open = 0;        
        menubar->container->clicked(menubar->container, window, x, y);
    } else {
        for (int i = 0; i < menubar->menu_count; i++) {
            menu_t* menu = menubar->menus[i];
            if (menu->is_open) {
                menu->container->clicked(menu->container, window, x, y);
                for (int i = 0; i < menubar->menu_count; i++) menubar->menus[i]->is_open = 0;
            }
        }
    }
}

void menu_clicked(widget_t* widget, window_t* window, int x, int y) {
    menu_t* menu = widget->extra_data;
    menubar_t* menubar = menu->widget->extra_data_2;
    menu->is_open = 0;
}

widget_t* menubar_init() {
    menubar_t* menubar = malloc(sizeof(menubar_t));

    menubar->container  = frame_init();
    menubar->menu_count = 0;
    menubar->menus      = malloc(1);
    menubar->widget     = widget_init();
    menubar->next_menu_x= 0;

    menubar->widget->extra_data = menubar;
    menubar->widget->draw       = menubar_draw;
    menubar->widget->clicked    = menubar_clicked;

    return menubar->widget;
}

void menubar_button_clicked(widget_t* widget, window_t* window) {
    button_t* button = widget->extra_data;
    menu_t* menu = widget->extra_data_2;
    menubar_t* menubar = menu->widget->extra_data_2;
    menu->is_open = 1;
    menu->container->draw(menu->container, window);
    menubar->widget->height = 10000;
}

void menu_button_clicked(widget_t* widget, window_t* window) {
    menubutton_t* btn = widget->extra_data_2;

    btn->clicked();
}

menu_t* menubar_add_menu(widget_t* widget, char* name) {
    menubar_t* menubar = widget->extra_data;
    menu_t* menu = malloc(sizeof(menu_t));
    
    menu->next_button_y= 0;
    menu->open_button  = button_init();
    menu->button_count = 0;
    menu->container    = frame_init();
    menu->is_open      = 0;
    menu->buttons      = malloc(1);
    menu->widget       = widget_init();
    
    menu->widget->extra_data = menu;
    menu->widget->extra_data_2 = menubar;
    menu->widget->clicked    = menu_clicked;

    menubar->menu_count++;
    menubar->menus = realloc(menubar->menus, sizeof(void*) * menubar->menu_count);
    menubar->menus[menubar->menu_count - 1] = menu;
    
    button_t* obtn = menu->open_button->extra_data;
    obtn->text = malloc(strlen(name));
    obtn->type = BUTTON_TEXT;

    menu->open_button->width = strlen(obtn->text) * 10;
    menu->open_button->height = 20;
    menu->open_button->clicked = menubar_button_clicked;
    menu->open_button->extra_data_2 = menu;

    strcpy(obtn->text, name);

    frame_managed_t* item = frame_add_item(menubar->container->extra_data, menu->open_button);
    item->x = menubar->next_menu_x;
    item->y = 0;

    menubar->next_menu_x += obtn->widget->width;

    menu->container->y = 20;

    return menu;
}

menubutton_t* menu_add_button(menu_t* menu, char* name, void(*clicked)()) {
    menu->button_count++;
    menu->buttons = realloc(menu->buttons, sizeof(void*) * menu->button_count);
    
    menubutton_t* button = malloc(sizeof(menubutton_t));

    button->button = button_init();
    button->clicked = clicked;
    button->widget = widget_init();
    button->widget->extra_data_2 = menu;
    button->button->clicked = menu_button_clicked;
    button->button->height = 20;
    button->button->width = strlen(name) * 10;

    button_t* btn = button->button->extra_data;
    btn->text = malloc(strlen(name));
    btn->type = BUTTON_TEXT;
    strcpy(btn->text, name);

    frame_managed_t* item = frame_add_item(menu->container->extra_data, button->button);
    item->x = 0;
    item->y = menu->next_button_y;

    menu->next_button_y += 20;

    button->button->extra_data_2 = button;
    button->button->extra_data_3 = item;

    return button;
}
