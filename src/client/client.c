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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ui/window.h>
#include <ui/widgets/button.h>
#include <ui/widgets/menubar.h>
#include <ui/widgets/scrollpane.h>
#include <ui/widgets/textbox.h>
#include <ui/widgets/label.h>
#include <ui/widgets/frame.h>
#include <common/util.h>
#include <common/args.h>
#include <common/attrib.h>
#include <config_parser/config.h>
#include <networking/networking.h>
#include <networking/types.h>
#include <client/set_nickname_dialog.h>
#include <client/license_dialog.h>

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

#ifdef WIN32
WSADATA* wsadata;
#endif 

struct server {
    client_config_server_t* server;    
    widget_t* button;
};

struct server** servers;
int             server_count;
char**          labels;
int             label_count;

client_config_t* config;

widget_t* messages_thing;
widget_t* messagebox;
widget_t* stripw;
menubar_t* stripe;
menu_t* filemenu;
menu_t* servermenu; 
menu_t* helpmenu;
window_t* main_window;
widget_t* serverlistw;
scroll_pane_t* serverliste;
widget_t* serverlistcollapsebtnw;
button_t* serverlistcollapsebtne;
widget_t* messagesw;
scroll_pane_t* messagese;
textbox_t* messageboxe;
widget_t* dialogthingw;
frame_t* dialogthinge;
widget_t* dialogbgw;
button_t* dialogbge;

// server connection
int sc;
int sc_connected;

int nextpos = 0;

void exit_button_clicked() {
    PINFO("user requested exit. goodbye\n");
    main_window->should_exit = 1;
}

int server_list_collapse_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    PINFO("server list collapsed\n");
    return 1;
}

int server_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    for (int i = 0; i < server_count; i++) {
        if (servers[i]->button == widget) {
            PINFO("connecting to server %s\n", servers[i]->server->name);
            
            if ((sc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                sc = 0;

#ifdef WIN32
                PFATL("socket(): %i\n", WSAGetLastError());
#else
                PFATL("%s\n", format_last_error());
#endif
                return 1;
            }

            struct sockaddr_in* addr = malloc(sizeof(struct sockaddr_in));

            addr->sin_family = AF_INET;
            addr->sin_port   = htons(servers[i]->server->port);

            inet_pton(AF_INET, servers[i]->server->host, &addr->sin_addr);

            if (connect(sc, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1) {
                sc = 0;
#ifdef WIN32
                PFATL("connect(): %s", format_error(WSAGetLastError()));
#else
                PFATL("%s\n", format_last_error());
#endif
                free(addr);
                return 1;
            }

            hello_t* hi = malloc(sizeof(hello_t));
            memset(hi, 0, sizeof(hello_t));

            hi->has_ident = 1; // we have an identity :)
            hi->ident     = "xutils ircreborn client";

            send_hello(sc, hi);

            if (servers[i]->server->nick) {
                set_nickname_t* setn = malloc(sizeof(set_nickname_t));
                setn->nickname = servers[i]->server->nick;

                send_set_nickname(sc, setn);

                free(setn);
            }

            sc_connected = 1;

            free(addr);
            free(hi);
            return 1;
        }
    }

    return 1;
}

struct server* server_list_add_server(widget_t* serverlist, client_config_server_t* server) {
    struct server* s = malloc(sizeof(struct server));
    s->button = button_init();
    s->server = server;
    button_t* btn = s->button->extra_data;

    s->button->height = 20;
    s->button->width  = serverlist->width - 20;
    s->button->clicked= &server_button_clicked;

    button_set_type(s->button, BUTTON_TEXT);
    button_set_text(s->button, server->name);

    scroll_pane_item_t* item = scroll_pane_add_item(serverlist, s->button);
    item->x = 0;
    item->y = nextpos;
    nextpos += 20;

    server_count++;
    servers = realloc(servers, sizeof(void*) * server_count);
    servers[server_count - 1] = s;

    return s;
}

void message_submit(widget_t* tb, window_t* window, char* text, int len) {
    if (sc_connected) {
        message_t* msg = malloc(sizeof(message_t));

        msg->message = malloc(len + 1);
        msg->name    = ""; // ignored

        memset(msg->message, 0,    len + 1);
        memcpy(msg->message, text, len);

        send_message(sc, msg);

        free(msg->message);
        free(msg);
    }
}

void client_add_message(window_t* window, char* message, char* name) {
    widget_t* msglw  = label_init();
    widget_t* namelw = label_init();

    label_t*  msgle  = msglw->extra_data;
    label_t*  namele = namelw->extra_data;


    msglw->width = strlen(message) * 10;
    msglw->height = 20;
    
    namelw->width = config->nickname_width;
    namelw->height = 20;

    label_set_text(msglw, message);
    label_set_text(namelw, name);

    scroll_pane_item_t* msgli  = scroll_pane_add_item(messages_thing, msglw);
    scroll_pane_item_t* nameli = scroll_pane_add_item(messages_thing, namelw);

    msgli->x = config->nickname_width;
    msgli->y = label_count * 20;

    nameli->x = 0;
    nameli->y = label_count * 20;

    label_count++;

    if (label_count * 20 > messages_thing->height) {
        ((scroll_pane_t*)messages_thing->extra_data)->pos = -(label_count * 20 - messages_thing->height);
    }

    messages_thing->draw(messages_thing, window);
}

void client_run_tasks(window_t* window) {
    if (sc_connected) {
#ifdef WIN32
        unsigned long data = 0;
        ioctlsocket(sc, FIONREAD, &data);
#else
        int data = 0;
        ioctl(sc, FIONREAD, &data);
#endif
        if (data) {
            char* head = malloc(9);
            memset(head, 0, 9);
            recv(sc, head, 8, 0);

            int op  = read_int(head);
            int len = read_int(head + 4);

            // the nice thing about doing it this way, is even 
            // if the client gets a message it doesn't recognize,
            // it wont crash
            char* body = malloc(len + 1);
            memset(body, 0, len + 1);
            recv(sc, body, len, 0);

            if (op == OPCODE_MESSAGE) {
                nstring_t* msg  = read_string(body);
                nstring_t* name = read_string(body + 4 + msg->len); 

                client_add_message(window, msg->str, name->str);

                free(msg->str);
                free(msg);
                free(name->str);
                free(name);
            } else if (op == OPCODE_SET_NICKNAME) {
                nstring_t* nick = read_string(body);
                
                char* notify_message = malloc(SSTRLEN("you are now known as \"") + nick->len + sizeof("\""));
                sprintf(notify_message, "you are now known as \"%s\"", nick->str);

                client_add_message(window, notify_message, " == ");
                
                free(nick->str);
                free(nick);
                free(notify_message);
            }

            free(body);
            free(head);
        }
    }
}

void client_recalculate_sizes(window_t* window) {
    stripw->x = 0;
    stripw->y = 0;
    stripw->width = window->width;
    stripw->height = 20;
    
    serverlistw->x = 0;
    serverlistw->y = 20;
    serverlistw->width = 200;
    serverlistw->height = window->height - serverlistw->y;
    
    messagesw->x = serverlistw->x + serverlistw->width;
    messagesw->y = 20;
    messagesw->width = window->width - messagesw->x;
    messagesw->height = window->height - messagesw->y - 20;

    messagebox->x = serverlistw->x + serverlistw->width;
    messagebox->y = window->height - 20;
    messagebox->width = window->width - messagebox->x;
    messagebox->height = 20;

    for (int i = 0; i < server_count; i++) {
        servers[i]->button->width = serverlistw->width - 20;
    }

/*
    serverlistcollapsebtnw->x = 200;
    serverlistcollapsebtnw->y = 20;
    serverlistcollapsebtnw->width = 20;
    serverlistcollapsebtnw->height = 20;
*/
}

void client_main() {
#ifdef WIN32
    wsadata = malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        PFATL("failed to start winsock, aborting\n");
        PFATL("%s", format_error(WSAGetLastError()));
        exit(1);
    }
#endif
    char* config_path = args_config_path;
    if (config_path == 0) {
        config_path = malloc(255);
        memset(config_path, 0, 255);
#ifdef WIN32
        strcat(config_path, getenv("USERPROFILE"));
#else
        strcat(config_path, getenv("HOME"));
#endif
        strcat(config_path, "/.ircreborn/client");
    }

    PINFO("reading config from %s\n", config_path);

    int configfd = open(config_path, O_RDONLY | O_CREAT);
    chmod(config_path, S_IWUSR | S_IRUSR);
    config = cfgparser_parse_client_config(configfd);
    close(configfd);
    free(config_path);

    servers = malloc(1);

    main_window = window_init();
    
    stripw    = menubar_init();
    stripe    = stripw->extra_data;
    stripw->z = 1000;    
    
    filemenu   = menubar_add_menu(stripw, "file");
    menu_add_button(filemenu, "exit", exit_button_clicked);
    servermenu = menubar_add_menu(stripw, "server");
    menu_add_button(servermenu, "set nickname", open_set_nickname_dialog);
    helpmenu  = menubar_add_menu(stripw, "help");
    menu_add_button(helpmenu, "license", open_license_dialog);    

    serverlistw = scroll_pane_init();
    serverliste = serverlistw->extra_data;
    messagesw = scroll_pane_init();
    messagese = messagesw->extra_data;
    messagebox = textbox_init();
    messageboxe = messagebox->extra_data;
    messages_thing = messagesw;
    
    messageboxe->submit = &message_submit;
        
    main_window->handle_bg_tasks = &client_run_tasks;
    main_window->resized         = &client_recalculate_sizes;

    window_add_widget(main_window, stripw);
    window_add_widget(main_window, serverlistw);
    window_add_widget(main_window, messagebox);
    window_add_widget(main_window, messagesw);

    for (int i = 0; i < config->server_count; i++) {
        server_list_add_server(serverlistw, config->servers[i]);
    }

    window_display(main_window);
    
    // cleanup
    menubar_free(stripw);
    for (int i = 0; i < serverliste->itemc; i++) {
        button_free(serverliste->items[i]->widget);
    }
    scroll_pane_free(serverlistw);
    for (int i = 0; i < messagese->itemc; i++) {
        label_free(messagese->items[i]->widget);
    }
    scroll_pane_free(messagesw);
    textbox_free(messagebox);
    window_free(main_window);
    for (int i = 0; i < server_count; i++) {
        free(servers[i]);
    }
    free(servers);
    client_config_free(config);
}
