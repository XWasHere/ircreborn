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

#include <ui/widgets/label.h>
#include <ui/widgets/button.h>
#include <ui/window.h>
#include <client/client.h>

static int ok_clicked(button_t* widget, int x, int y) {
    widget->window->should_exit = 1;
    return 1;
}

void open_license_dialog() {
    window_t* dialog;

    button_t* ok;
    label_t*  text0;
    label_t*  text1;
    label_t*  text2;

    dialog = new window_t();
    
    ok = new button_t();
    text0 = new label_t();
    text1 = new label_t();
    text2 = new label_t();

    text0->set_text("This program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.");
    text0->bg_color = get_node_rgb(config->theme, "common.primary_color");
    text0->text_color = get_node_rgb(config->theme, "common.text_color");
    text1->set_text("This program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.");
    text1->bg_color = get_node_rgb(config->theme, "common.primary_color");
    text1->text_color = get_node_rgb(config->theme, "common.text_color");
    text2->set_text("You should have received a copy of the GNU General Public License\nalong with this program.  If not, see <https://www.gnu.org/licenses/>.");
    text2->bg_color = get_node_rgb(config->theme, "common.primary_color");
    text2->text_color = get_node_rgb(config->theme, "common.text_color");

    text0->x = 10;
    text0->y = 10;
    text0->width = 600;
    text0->height = 80;
    text0->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    text1->x = 10;
    text1->y = 100;
    text1->width = 600;
    text1->height = 80;
    text1->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    text2->x = 10;
    text2->y = 190;
    text2->width = 600;
    text2->height = 40;
    text2->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;
    ok->x    = 280;
    ok->y    = 240;
    ok->width  = 40;
    ok->height = 20;

    ok->type = BUTTON_TEXT;
    ok->set_text("cool");
    ok->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    ok->text_color = get_node_rgb(config->theme, "common.text_color");
    ok->on_clicked = ok_clicked;

    dialog->add_widget(ok);
    dialog->add_widget(text0);
    dialog->add_widget(text1);
    dialog->add_widget(text2);

    dialog->set_type(WINDOW_WM_TYPE_DIALOG);
    dialog->set_size(620, 270);
    dialog->bg_color = get_node_rgb(config->theme, "common.primary_color");
    dialog->show(1);

    delete dialog;
}