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

#include <main.h>
#include <client/client.h>
#include <ui/window.h>
#include <ui/widgets/label.h>
#include <ui/widgets/textbox.h>
#include <ui/widgets/button.h>
#include <common/util.h>
#include <common/logger.h>
#include <networking/networking.h>
#include <stdlib.h>

static window_t* dialog;

static label_t* text;
static textbox_t* entry;
static button_t* ok;
static button_t* cancel;

static int lock = 0;

static int set_nickname_dialog_cancel_clicked(button_t* widget, int x, int y) {
    dialog->should_exit = 1;
}

static int set_nickname_dialog_ok_clicked(button_t* widget, int x, int y) {
    if (connection != 0) {
        ircreborn_pset_nickname_t packet;
        packet.nickname        = entry->text;
        packet.nickname_length = strlen(entry->text);
        connection->send_set_nickname(&packet);
    }
    
    dialog->should_exit = 1;

    return 1;
}

void open_set_nickname_dialog() {
    if (lock) return;
    lock = 1;
    dialog = new window_t();
    
    dialog->set_type(WINDOW_WM_TYPE_DIALOG);
    dialog->bg_color = get_node_rgb(config->theme, "common.primary_color");

    if (connection != 0) {
        text   = new label_t();
        entry  = new textbox_t();
        ok     = new button_t();
        cancel = new button_t();

        text->x      = 110;
        text->y      = 10;
        text->width  = 120;
        text->height = 20;
        text->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;

        entry->x      = 10;
        entry->y      = 40;
        entry->width  = 320;
        entry->height = 20;

        ok->x       = 120;
        ok->y       = 70;
        ok->width   = 20;
        ok->height  = 20;
        ok->on_clicked = set_nickname_dialog_ok_clicked;

        cancel->x       = 150;
        cancel->y       = 70;
        cancel->width   = 60;
        cancel->height  = 20;
        cancel->on_clicked = set_nickname_dialog_cancel_clicked;

        entry->bg_color = get_node_rgb(config->theme, "common.secondary_color");
        entry->border_color = get_node_rgb(config->theme, "common.secondary_color");
        entry->text_color = get_node_rgb(config->theme, "common.text_color");

        text->set_text("set nickname");
        text->bg_color = get_node_rgb(config->theme, "common.primary_color");
        text->text_color = get_node_rgb(config->theme, "common.text_color");

        ok->type = BUTTON_TEXT;
        ok->set_text("ok");
        ok->bg_color = get_node_rgb(config->theme, "common.secondary_color");
        ok->text_color = get_node_rgb(config->theme, "common.text_color");

        cancel->type = BUTTON_TEXT;
        cancel->set_text("cancel");
        cancel->bg_color = get_node_rgb(config->theme, "common.secondary_color");
        cancel->text_color = get_node_rgb(config->theme, "common.text_color");

        dialog->add_widget(text);
        dialog->add_widget(entry);
        dialog->add_widget(ok);
        dialog->add_widget(cancel);

        dialog->set_size(340, 100);
        
        dialog->show(1);

//        text->~t\w\;
//        entry;
//        ok;
 //       cancel;

        delete dialog;
    } else {
        logger->log(CHANNEL_WARN, "connect to a server before setting your nickname you phycopath\n");

        text   = new label_t();
        ok     = new button_t();

        text->x      = 10;
        text->y      = 10;
        text->width  = 300;
        text->height = 20;
        text->style = STYLE_NBB | STYLE_NBL | STYLE_NBR | STYLE_NBT;

        ok->x        = 110;
        ok->y        = 40;
        ok->width    = 90;
        ok->height   = 20;
        ok->on_clicked  = set_nickname_dialog_ok_clicked;

        text->set_text("you need to connect to a server first");
        text->bg_color = get_node_rgb(config->theme, "common.primary_color");
        text->text_color = get_node_rgb(config->theme, "common.text_color");

        ok->type = BUTTON_TEXT;
        ok->set_text("ok");
        ok->bg_color = get_node_rgb(config->theme, "common.secondary_color");
        ok->text_color = get_node_rgb(config->theme, "common.text_color");

        dialog->add_widget(text);
        dialog->add_widget(ok);

        dialog->set_size(320, 70);
        dialog->show(1);

        delete dialog;
    }
    lock = 0;
}