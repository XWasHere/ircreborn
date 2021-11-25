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

#include <ui/widgets/label.h>
#include <ui/widgets/button.h>
#include <ui/window.h>
#include <ui/widget.h>

static window_t* dialog;

static widget_t* okw;
static widget_t* text0w; // TODO: holy shit add newline support
static widget_t* text1w;
static widget_t* text2w;
static widget_t* text3w;
static widget_t* text4w;
static widget_t* text5w;
static widget_t* text6w;
static widget_t* text7w;
static widget_t* text8w;
static widget_t* text9w;

static button_t* oke;
static label_t*  text0e;
static label_t*  text1e;
static label_t*  text2e;
static label_t*  text3e;
static label_t*  text4e;
static label_t*  text5e;
static label_t*  text6e;
static label_t*  text7e;
static label_t*  text8e;
static label_t*  text9e;

static int ok_clicked(widget_t* widget, window_t* window, int x, int y) {
    dialog->should_exit = 1;
    return 1;
}

void open_license_dialog() {
    dialog = window_init();
    
    okw = button_init();
    oke = okw->extra_data;
    
    text0w = label_init();
    text0e = text0w->extra_data;    
    text1w = label_init();
    text1e = text1w->extra_data;    
    text2w = label_init();
    text2e = text2w->extra_data;    
    text3w = label_init();
    text3e = text3w->extra_data;    
    text4w = label_init();
    text4e = text4w->extra_data;    
    text5w = label_init();
    text5e = text5w->extra_data;    
    text6w = label_init();
    text6e = text6w->extra_data;    
    text7w = label_init();
    text7e = text7w->extra_data;    
    text8w = label_init();
    text8e = text8w->extra_data;    
    text9w = label_init();
    text9e = text9w->extra_data;

    label_set_text(text0w, "This program is free software: you can redistribute it and/or modify");
    label_set_text(text1w, "it under the terms of the GNU General Public License as published by");
    label_set_text(text2w, "the Free Software Foundation, either version 3 of the License, or");
    label_set_text(text3w, "(at your option) any later version.");
    label_set_text(text4w, "This program is distributed in the hope that it will be useful,");
    label_set_text(text5w, "but WITHOUT ANY WARRANTY; without even the implied warranty of");
    label_set_text(text6w, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
    label_set_text(text7w, "GNU General Public License for more details.");
    label_set_text(text8w, "You should have received a copy of the GNU General Public License");
    label_set_text(text9w, "along with this program.  If not, see <https://www.gnu.org/licenses/>.");

    text0w->x = 10;
    text0w->y = 10;
    text0w->width = 600;
    text0w->height = 20;
    text1w->x = 10;
    text1w->y = 30;
    text1w->width = 600;
    text1w->height = 20;
    text2w->x = 10;
    text2w->y = 50;
    text2w->width = 600;
    text2w->height = 20;
    text3w->x = 10;
    text3w->y = 70;
    text3w->width = 600;
    text3w->height = 20;
    text4w->x = 10;
    text4w->y = 100;
    text4w->width = 600;
    text4w->height = 20;
    text5w->x = 10;
    text5w->y = 120;
    text5w->width = 600;
    text5w->height = 20;
    text6w->x = 10;
    text6w->y = 140;
    text6w->width = 600;
    text6w->height = 20;
    text7w->x = 10;
    text7w->y = 160;
    text7w->width = 600;
    text7w->height = 20;
    text8w->x = 10;
    text8w->y = 190;
    text8w->width = 600;
    text8w->height = 20;
    text9w->x = 10;
    text9w->y = 210;
    text9w->width = 600;
    text9w->height = 20;
    okw->x    = 280;
    okw->y    = 240;
    okw->width  = 40;
    okw->height = 20;

    button_set_type(okw, BUTTON_TEXT);
    button_set_text(okw, "cool");
    
    okw->clicked = ok_clicked;

    window_add_widget(dialog, okw);
    window_add_widget(dialog, text0w);
    window_add_widget(dialog, text1w);
    window_add_widget(dialog, text2w);
    window_add_widget(dialog, text3w);
    window_add_widget(dialog, text4w);
    window_add_widget(dialog, text5w);
    window_add_widget(dialog, text6w);
    window_add_widget(dialog, text7w);
    window_add_widget(dialog, text8w);
    window_add_widget(dialog, text9w);

    window_set_type(dialog, WINDOW_WM_TYPE_DIALOG);
    window_set_size(dialog, 620, 270);
    window_display(dialog);

    label_free(text0w);
    label_free(text1w);
    label_free(text2w);
    label_free(text3w);
    label_free(text4w);
    label_free(text5w);
    label_free(text6w);
    label_free(text7w);
    label_free(text8w);
    label_free(text9w);
    button_free(okw);

    window_free(dialog);
}