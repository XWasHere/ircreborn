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
#include <common/util.h>
#include <common/args.h>
#include <config_parser/config.h>
#include <networking/networking.h>
#include <networking/types.h>

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
    char* host;
    int   port;
    char* name;
    
    widget_t* button;
};

struct server** servers;
int             server_count;
char**          labels;
int             label_count;


widget_t* messages_thing;
widget_t* messagebox;
widget_t* stripw;
menubar_t* stripe;
menu_t* filemenu;
menu_t* servermenu; 
window_t* main_window;
widget_t* serverlistw;
scroll_pane_t* serverliste;
widget_t* serverlistcollapsebtnw;
button_t* serverlistcollapsebtne;
widget_t* messagesw;
scroll_pane_t* messagese;
textbox_t* messageboxe;

// server connection
int sc;
int sc_connected;

int nextpos = 0;

void exit_button_clicked() {
    printf(FMT_INFO("user requested exit. goodbye\n"));
    exit(0);
}

void server_list_collapse_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    printf(FMT_INFO("server list collapsed\n"));
}

void server_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    for (int i = 0; i < server_count; i++) {
        if (servers[i]->button == widget) {
            printf(FMT_INFO("connecting to server %s\n"), servers[i]->name);
            
            if ((sc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                sc = 0;

#ifdef WIN32
                printf(FMT_FATL("socket(): %i\n"), WSAGetLastError());
#else
                printf(FMT_FATL("%s\n"), format_last_error());
#endif
                return;
            }

            struct sockaddr_in* addr = malloc(sizeof(struct sockaddr_in));

            addr->sin_family = AF_INET;
            addr->sin_port   = htons(servers[i]->port);

            inet_pton(AF_INET, servers[i]->host, &addr->sin_addr);

            if (connect(sc, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1) {
                sc = 0;

#ifdef WIN32
                printf(FMT_FATL("connect(): %s"), format_error(WSAGetLastError()));
#else
                printf(FMT_FATL("%s\n"), format_last_error());
#endif

                return;
            }

            hello_t* hi = malloc(sizeof(hello_t));
            memset(hi, 0, sizeof(hello_t));

            hi->has_ident = 1; // we have an identity :)
            hi->ident     = "xutils ircreborn client";

            send_hello(sc, hi);

            sc_connected = 1;

            return;
        }
    }
}

struct server* server_list_add_server(widget_t* serverlist, char* name, char* host, int port) {
    struct server* s = malloc(sizeof(struct server));
    s->button = button_init();
    s->name = name;
    s->host = host;
    s->port = port;
    button_t* btn = s->button->extra_data;

    s->button->height = 20;
    s->button->width  = serverlist->width - 20;
    s->button->clicked= &server_button_clicked;

    btn->type = BUTTON_TEXT;
    btn->text = name;

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
        memset(msg->message, 0,    len + 1);
        memcpy(msg->message, text, len);

        send_message(sc, msg);

        free(msg->message);
        free(msg);
    }
}

void client_add_message(window_t* window, char* message) {
    widget_t* label = label_init();
    label_t*  lab   = label->extra_data;

    lab->text = message;
    lab->len = strlen(message);
    
    label->width = strlen(message) * 10;
    label->height = 20;

    scroll_pane_item_t* spi = scroll_pane_add_item(messages_thing, label);
    spi->x = 0;
    spi->y = label_count * 20;

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
                nstring_t* msg = read_string(body);

                client_add_message(window, msg->str);
            } else if (op == OPCODE_SET_NICKNAME) {
                nstring_t* nick = read_string(body);
                
                char* notify_message = malloc(sizeof("you are now known as \"") - 1 + nick->len + sizeof("\""));
                sprintf(notify_message, "you are now known as \"%s\"", nick->str);

                client_add_message(window, notify_message);
            }
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

void open_set_nickname_dialog() {
    printf(FMT_INFO("if i wasn't lazy, this would set your nickname\n"));
}

void client_main() {
#ifdef WIN32
    wsadata = malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        printf(FMT_FATL("failed to start winsock, aborting\n"));
        printf(FMT_FATL("%s"), format_error(WSAGetLastError()));
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

    printf(FMT_INFO("reading config from %s\n"), config_path);

    int configfd = open(config_path, O_RDONLY | O_CREAT);
    chmod(config_path, S_IWUSR | S_IRUSR);
    client_config_t* config = cfgparser_parse_client_config(configfd);
    close(configfd);

    servers = malloc(1);

    main_window = window_init();
    
    stripw = menubar_init();
    stripe = stripw->extra_data;
    
    filemenu   = menubar_add_menu(stripw, "file");
    menu_add_button(filemenu, "exit", exit_button_clicked);
    servermenu = menubar_add_menu(stripw, "server");
    menu_add_button(servermenu, "set nickname", open_set_nickname_dialog);
    
    serverlistw = scroll_pane_init();
    serverliste = serverlistw->extra_data;
    serverlistcollapsebtnw = button_init();
    serverlistcollapsebtne = serverlistcollapsebtnw->extra_data;
    messagesw = scroll_pane_init();
    messagese = messagesw->extra_data;
    messagebox = textbox_init();
    messageboxe = messagebox->extra_data;
    messages_thing = messagesw;
    
    messageboxe->submit = &message_submit;
    
    serverlistcollapsebtnw->clicked = &server_list_collapse_button_clicked;
    serverlistcollapsebtne->type = BUTTON_TEXT;
    serverlistcollapsebtne->text = "<";
    
    main_window->handle_bg_tasks = &client_run_tasks;
    main_window->resized         = &client_recalculate_sizes;

    window_add_widget(main_window, stripw);
    window_add_widget(main_window, serverlistw);
    window_add_widget(main_window, messagebox);
    window_add_widget(main_window, messagesw);

    for (int i = 0; i < config->server_count; i++) {
        server_list_add_server(serverlistw, config->servers[i]->name, config->servers[i]->host, config->servers[i]->port);
    }

    window_display(main_window);
}
