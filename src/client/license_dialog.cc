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
#include <client/client.h>

static window_t* dialog;

static widget_t* okw;
static widget_t* text0w; // TODO: holy shit add newline support
static widget_t* text1w;
static widget_t* text2w;

static button_t* oke;
static label_t*  text0e;
static label_t*  text1e;
static label_t*  text2e;

static int lock = 0;

static int ok_clicked(widget_t* widget, window_t* window, int x, int y) {
    dialog->should_exit = 1;
    return 1;
}

void open_license_dialog() {
    if (lock) return;
    lock = 1;

    dialog = window_init();
    
    okw = button_init();
    oke = (button_t*)okw->extra_data;
    
    text0w = label_init();
    text0e = (label_t*)text0w->extra_data;    
    text1w = label_init();
    text1e = (label_t*)text1w->extra_data;    
    text2w = label_init();
    text2e = (label_t*)text2w->extra_data;

    label_set_text(text0w, "This program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.");
    label_set_color(
        text0w,
        LABEL_BG_COLOR,
        get_node_rgb(config->theme, "common.primary_color")
    );
    label_set_color(
        text0w,
        LABEL_TEXT_COLOR,
        get_node_rgb(config->theme, "common.text_color")
    );
    label_set_text(text1w, "This program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.");
    label_set_color(
        text1w,
        LABEL_BG_COLOR,
        get_node_rgb(config->theme, "common.primary_color")
    );
    label_set_color(
        text1w,
        LABEL_TEXT_COLOR,
        get_node_rgb(config->theme, "common.text_color")
    );
    label_set_text(text2w, "You should have received a copy of the GNU General Public License\nalong with this program.  If not, see <https://www.gnu.org/licenses/>.");
    label_set_color(
        text2w,
        LABEL_BG_COLOR,
        get_node_rgb(config->theme, "common.primary_color")
    );
    label_set_color(
        text2w,
        LABEL_TEXT_COLOR,
        get_node_rgb(config->theme, "common.text_color")
    );

    text0w->x = 10;
    text0w->y = 10;
    text0w->width = 600;
    text0w->height = 80;
    text0w->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    text1w->x = 10;
    text1w->y = 100;
    text1w->width = 600;
    text1w->height = 80;
    text1w->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    text2w->x = 10;
    text2w->y = 190;
    text2w->width = 600;
    text2w->height = 40;
    text2w->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    okw->x    = 280;
    okw->y    = 240;
    okw->width  = 40;
    okw->height = 20;

    button_set_type(okw, BUTTON_TEXT);
    button_set_text(okw, "cool");
    button_set_color(
        okw,
        BUTTON_COLOR_BG,
        get_node_rgb(config->theme, "common.secondary_color")
    );
    button_set_color(
       okw, 
       BUTTON_COLOR_TX, 
       get_node_rgb(config->theme, "common.text_color")
    );

    okw->clicked = ok_clicked;

    window_add_widget(dialog, okw);
    window_add_widget(dialog, text0w);
    window_add_widget(dialog, text1w);
    window_add_widget(dialog, text2w);

    window_set_type(dialog, WINDOW_WM_TYPE_DIALOG);
    window_set_size(dialog, 620, 270);
    window_set_bg(
        dialog,
        get_node_rgb(config->theme, "common.primary_color")
    );
    window_display(dialog, 1);

    label_free(text0w);
    label_free(text1w);
    label_free(text2w);
    button_free(okw);

    window_free(dialog);
    
    lock = 0;
}