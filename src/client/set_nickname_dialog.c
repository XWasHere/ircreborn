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

#include <client/client.h>
#include <ui/window.h>
#include <ui/widgets/label.h>
#include <ui/widgets/textbox.h>
#include <ui/widgets/button.h>
#include <common/util.h>
#include <networking/networking.h>

window_t* dialog;

widget_t* textw;
widget_t* entryw;
widget_t* okw;
widget_t* cancelw;

label_t* texte;
textbox_t* entrye;
button_t* oke;
button_t* cancele;

int set_nickname_dialog_cancel_clicked() {
    dialog->should_exit = 1;

    return 1;
}

int set_nickname_dialog_ok_clicked() {
    if (sc_connected) {
        set_nickname_t* packet = malloc(sizeof(set_nickname_t));
        
        packet->nickname = entrye->text;
        
        send_set_nickname(sc, packet);
        
        free(packet);
    }
    
    dialog->should_exit = 1;

    return 1;
}

void open_set_nickname_dialog() {
    dialog = window_init();
    
    window_set_type(dialog, WINDOW_WM_TYPE_DIALOG);

    if (sc_connected) {
        textw   = label_init();
        entryw  = textbox_init();
        okw     = button_init();
        cancelw = button_init();

        texte   = textw->extra_data;
        entrye  = entryw->extra_data;
        oke     = okw->extra_data;
        cancele = cancelw->extra_data;

        textw->x      = 110;
        textw->y      = 10;
        textw->width  = 120;
        textw->height = 20;

        entryw->x      = 10;
        entryw->y      = 40;
        entryw->width  = 320;
        entryw->height = 20;

        okw->x       = 120;
        okw->y       = 70;
        okw->width   = 20;
        okw->height  = 20;
        okw->clicked = set_nickname_dialog_ok_clicked;

        cancelw->x       = 150;
        cancelw->y       = 70;
        cancelw->width   = 60;
        cancelw->height  = 20;
        cancelw->clicked = set_nickname_dialog_cancel_clicked;

        texte->text = "set nickname";
        texte->len  = strlen(texte->text);
        
        oke->text = "ok";
        oke->type = BUTTON_TEXT;

        cancele->text = "cancel";
        cancele->type = BUTTON_TEXT; 

        window_add_widget(dialog, textw);
        window_add_widget(dialog, entryw);
        window_add_widget(dialog, okw);
        window_add_widget(dialog, cancelw);

        window_set_size(dialog, 340, 100);
    } else {
        printf(FMT_WARN("connect to a server before setting your nickname you phycopath\n"));

        textw   = label_init();
        okw     = button_init();

        texte = textw->extra_data;
        oke   = okw->extra_data;

        textw->x      = 10;
        textw->y      = 10;
        textw->width  = 300;
        textw->height = 20;
        
        okw->x        = 110;
        okw->y        = 40;
        okw->width    = 90;
        okw->height   = 20;
        okw->clicked  = set_nickname_dialog_ok_clicked;

        texte->text = "you need to connect to a server first";
        texte->len  = strlen(texte->text);
        
        oke->text   = "well shit";
        oke->type   = BUTTON_TEXT;
        
        window_add_widget(dialog, textw);
        window_add_widget(dialog, okw);

        window_set_size(dialog, 320, 70);
    }
    
    window_display(dialog);
}