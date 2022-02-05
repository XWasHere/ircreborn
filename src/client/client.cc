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
#include <main.h>
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
#include <common/logger.h>
#include <config_parser/config.h>
#include <config_parser/theme.h>
#include <networking/networking.h>
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

uint32_t allowed_protocols[] = { 1U };

struct server {
    client_config_server_t* server;    
    button_t* button;
};

struct server** servers;
int             server_count;
char**          labels;
int             label_count;

client_config_t* config;

scroll_pane_t* messages_thing;
menubar_t* strip;
menu_t* filemenu;
menubutton_t* exitbutton;
menu_t* servermenu; 
menubutton_t* setnicknamebutton;
menu_t* helpmenu;
menubutton_t* licensebutton;
window_t* main_window;
scroll_pane_t* serverlist;
widget_t* serverlistcollapsebtnw;
button_t* serverlistcollapsebtne;
scroll_pane_t* messages;
textbox_t* messagebox;
frame_t* dialogthinge;
button_t* dialogbg;

// server connection
int connection_state = 0;
char* await_set_nickname_data = 0;
ircreborn_connection_t* connection = 0;

int nextpos = 0;

void exit_button_clicked() {
    logger->log(CHANNEL_DBUG, "user requested exit. goodbye\n");
    main_window->should_exit = 1;
}

int server_list_collapse_button_clicked(widget_t* widget, window_t* window, int x, int y) {
    logger->log(CHANNEL_DBUG, "server list collapsed\n");
    return 1;
}

int server_button_clicked(button_t* widget, int x, int y) {
    for (int i = 0; i < server_count; i++) {
        if (servers[i]->button == widget) {
            int fd;

            logger->log(CHANNEL_DBUG, "connecting to server %s\n", servers[i]->server->name);
            
            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                fd = 0;

#ifdef WIN32
                logger->log(CHANNEL_FATL, "socket(): %i\n", WSAGetLastError());
#else
                logger->log(CHANNEL_FATL, "%s\n", format_last_error());
#endif
                return 1;
            }

            struct sockaddr_in* addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

            addr->sin_family = AF_INET;
            addr->sin_port   = htons(servers[i]->server->port);

            inet_pton(AF_INET, servers[i]->server->host, &addr->sin_addr);

            if (connect(fd, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1) {
                fd = 0;
#ifdef WIN32
                logger->log(CHANNEL_FATL, "connect(): %s", format_error(WSAGetLastError()));
#else
                logger->log(CHANNEL_FATL, "%s\n", format_last_error());
#endif
                free(addr);
                return 1;
            }

            connection              = new ircreborn_connection_t(fd);
            connection_state        = 0;
            await_set_nickname_data = 0;
            
            ircreborn_phello_t hello;
            hello.ident          = "IRCREBORN_REFERENCE_CLIENT";
            hello.ident_length   = 27;
            hello.protocols      = allowed_protocols;
            hello.protocol_count = 1;
            hello.master         = 0;
            connection->send_hello(&hello);

            if (servers[i]->server->nick) {
                await_set_nickname_data = servers[i]->server->nick;
            }

            free(addr);
            return 1;
        }
    }

    return 1;
}

struct server* server_list_add_server(scroll_pane_t* serverlist, client_config_server_t* server) {
    struct server* s = (struct server*)malloc(sizeof(struct server));
    s->button = new button_t();
    s->server = server;;

    s->button->height = 20;
    s->button->width  = serverlist->width - 20;
    s->button->on_clicked = &server_button_clicked;

/*    
    btn->bg_color.r = config->theme.server_list_item_bg_color.r;
    btn->bg_color.g = config->theme.server_list_item_bg_color.g;
    btn->bg_color.b = config->theme.server_list_item_bg_color.b;
    btn->text_color.r = config->theme.server_list_item_text_color.r;
    btn->text_color.g = config->theme.server_list_item_text_color.g;
    btn->text_color.b = config->theme.server_list_item_text_color.b;
    btn->border_color.r = config->theme.server_list_item_border_color.r;
    btn->border_color.g = config->theme.server_list_item_border_color.g;
    btn->border_color.b = config->theme.server_list_item_border_color.b;
*/

    s->button->bg_color = get_node_rgb(config->theme, "common.tertiary_color");
    s->button->text_color = get_node_rgb(config->theme, "common.text_color");
    // button_set_color(
        // btn,
        // BUTTON_COLOR_BR,
        // 0
    // );

    s->button->type = BUTTON_TEXT;
    s->button->set_text(server->name);

    scroll_pane_item_t* item = serverlist->add_item(s->button);
    item->x = 0;
    item->y = nextpos;
    nextpos += 20;

    server_count++;
    servers = (struct server**)realloc(servers, sizeof(void*) * server_count);
    servers[server_count - 1] = s;

    return s;
}

void message_submit(textbox_t* tb, char* text, int len) {
    if (connection != 0) {
        ircreborn_psend_message_t packet;
        packet.message        = text;
        packet.message_length = len;
        connection->send_send_message(&packet);

        tb->cursorpos = 0;
        tb->textlen = 0;
        tb->text[0] = 0; 
    }
}

void client_add_message(window_t* window, char* message, char* name) {
    label_t* msgl  = new label_t();
    label_t* namel = new label_t();

    msgl->style = STYLE_NBB | STYLE_NBR;
    namel->style = STYLE_NBB;

    int lines = 1;
    int len = strlen(message);
    for (int i = 0; i < len; i++) {
        if (message[i] == '\n') lines++;
    }

    msgl->width = strlen(message) * 10;
    msgl->height = 20 * lines;
    
    namel->width = config->nickname_width;
    namel->height = 20;

    msgl->set_text(message);
    msgl->bg_color = get_node_rgb(config->theme, "common.primary_color");
    msgl->text_color = get_node_rgb(config->theme, "common.text_color");
    namel->set_text(name);
    namel->bg_color = get_node_rgb(config->theme, "common.primary_color");
    namel->text_color = get_node_rgb(config->theme, "common.text_color");

    scroll_pane_item_t* msgli  = messages_thing->add_item(msgl);
    scroll_pane_item_t* nameli = messages_thing->add_item(namel);

    msgli->x = config->nickname_width;
    msgli->y = label_count * 20;

    nameli->x = 0;
    nameli->y = label_count * 20;

    label_count += lines;

    if (label_count * 20 > messages_thing->height) {
        messages_thing->pos = -(label_count * 20 - messages_thing->height);
    }

    messages_thing->draw();
}

void client_run_tasks(window_t* window) {
    if (connection != 0) {
#ifdef WIN32
        unsigned long data = 0;
        ioctlsocket(connection->fd, FIONREAD, &data);
#else
        int data = 0;
        ioctl(connection->fd, FIONREAD, &data);
#endif

        if (data) {
            connection->recv_packet();

            ircreborn_packet_t* packet = connection->queue_get(0);

            if (packet == 0) {
                delete connection;
                connection = 0;
                return;
            }
            
            if (packet->opcode == IRCREBORN_PROTO_V1_OP::HELLO) { 
                ircreborn_packet_t* p = connection->queue_get(1);

                free(p->payload);
                free(p);
            } else if (packet->opcode == IRCREBORN_PROTO_V1_OP::SET_PROTO) {
                ircreborn_pset_proto_t* p = connection->queue_get_set_proto(1);
                connection->protocol_version = p->protocol;
                logger->log(CHANNEL_DBUG, "using proto %i\n", connection->protocol_version);
                
                // if we just opened the connection, set the users nickname
                if (connection_state == 0) {
                    if (await_set_nickname_data) {
                        ircreborn_pset_nickname_t packet;
                        packet.nickname        = await_set_nickname_data;
                        packet.nickname_length = strlen(await_set_nickname_data);
                        connection->send_set_nickname(&packet);

                        connection_state = 1;
                    }
                }

                free(p);
            } else if (packet->opcode == IRCREBORN_PROTO_V1_OP::NICKNAME_UPDATED) {
                ircreborn_pnickname_updated_t* p = connection->queue_get_nickname_updated(1);

                char* notify_message = (char*)malloc(SSTRLEN("you are now known as \"") + p->nickname_length + sizeof("\""));
                sprintf(notify_message, "you are now known as \"%s\"", p->nickname);

                client_add_message(window, notify_message, " == ");
                
                free(p->nickname);
                free(p);
                free(notify_message);
            } else if (packet->opcode == IRCREBORN_PROTO_V1_OP::RECV_MESSAGE) {
                ircreborn_precv_message_t* p = connection->queue_get_recv_message(1);

                client_add_message(window, p->message, p->author);

                free(p);
            } else {
                free(connection->queue_get(1));
            }
        }
    }
}

void client_recalculate_sizes(window_t* window) {
    strip->x = 0;
    strip->y = 0;
    strip->width = window->width;
    strip->height = 20;
    
    serverlist->x = 0;
    serverlist->y = 20;
    serverlist->width = 200;
    serverlist->height = window->height - serverlist->y;
    
    messages->x = serverlist->x + serverlist->width;
    messages->y = 20;
    messages->width = window->width - messages->x;
    messages->height = window->height - messages->y - 20;

    char* t = messagebox->text;
    int tl =  messagebox->textlen;

    int mh = 20;

    for (int i = 0; i < tl; i++) {
        if (t[i] == '\n') mh += 20;
    }

    messagebox->x = serverlist->x + serverlist->width;
    messagebox->y = window->height - mh;
    messagebox->width = window->width - messagebox->x;
    messagebox->height = mh;

    for (int i = 0; i < server_count; i++) {
        servers[i]->button->width = serverlist->width - 20;
    }

/*
    serverlistcollapsebtnw->x = 200;
    serverlistcollapsebtnw->y = 20;
    serverlistcollapsebtnw->width = 20;
    serverlistcollapsebtnw->height = 20;
*/
}

void handle_mb_kp(textbox_t* tb, uint32_t key, uint16_t mod) {
    client_recalculate_sizes(tb->window);
}

void client_main() {
    theme_tree_init();
    register_theme_node("common", NODE_TYPE_BRANCH);
    register_theme_node("common.primary_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.primary_color", RGBA(0x000000ff));
    register_theme_node("common.secondary_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.secondary_color", RGBA(0x080808ff));
    register_theme_node("common.tertiary_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.tertiary_color", RGBA(0x1f1f1fff));
    register_theme_node("common.scrollbar_track_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.scrollbar_track_color", RGBA(0x000000ff));
    register_theme_node("common.scrollbar_button_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.scrollbar_button_color", RGBA(0x1f1f1fff));
    register_theme_node("common.scrollbar_thumb_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.scrollbar_thumb_color", RGBA(0x2f2f2fff));
    register_theme_node("common.text_color", NODE_TYPE_RGBA);
    set_node_default_rgb("common.text_color", RGBA(0xffffffff));

#ifdef WIN32
    wsadata = (WSADATA*)malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        logger->log(CHANNEL_FATL, "failed to start winsock, aborting\n");
        logger->log(CHANNEL_FATL, "%s", format_error(WSAGetLastError()));
        exit(1);
    }
#endif
    char* config_path = args_config_path;
    if (config_path == 0) {
        config_path = (char*)malloc(255);
        memset(config_path, 0, 255);
#ifdef WIN32
        strcat(config_path, getenv("USERPROFILE"));
#else
        strcat(config_path, getenv("HOME"));
#endif
        strcat(config_path, "/.ircreborn/client");
    }

    logger->log(CHANNEL_DBUG, "reading config from %s\n", config_path);

    int configfd = open(config_path, O_RDONLY | O_CREAT);
    chmod(config_path, S_IWUSR | S_IRUSR);
    config = cfgparser_parse_client_config(configfd);
    close(configfd);
    free(config_path);

    servers = (struct server**)malloc(1);

    main_window = new window_t();
    
    strip    = new menubar_t();
    strip->z = 1000;
    
    filemenu = strip->add_menu("file");
    exitbutton = filemenu->add_button("exit", exit_button_clicked);
    servermenu = strip->add_menu("server");
    setnicknamebutton = servermenu->add_button("set nickname", open_set_nickname_dialog);
    helpmenu = strip->add_menu("help");
    licensebutton = helpmenu->add_button("license", open_license_dialog);    
    
    strip->container->bg_color = get_node_rgb(config->theme, "common.primary_color");
    filemenu->open_button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    filemenu->open_button->text_color = get_node_rgb(config->theme, "common.text_color");
    exitbutton->button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    exitbutton->button->text_color = get_node_rgb(config->theme, "common.text_color");
    servermenu->open_button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    servermenu->open_button->text_color = get_node_rgb(config->theme, "common.text_color");
    setnicknamebutton->button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    setnicknamebutton->button->text_color = get_node_rgb(config->theme, "common.text_color");
    helpmenu->open_button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    helpmenu->open_button->text_color = get_node_rgb(config->theme, "common.text_color");
    licensebutton->button->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    licensebutton->button->text_color = get_node_rgb(config->theme, "common.text_color");

    serverlist = new scroll_pane_t();
    messages = new scroll_pane_t();
    messagebox = new textbox_t();
    messages_thing = messages;

    serverlist->bg_color = get_node_rgb(config->theme, "common.secondary_color");
    serverlist->button_color = get_node_rgb(config->theme, "common.scrollbar_button_color");
    serverlist->track_color = get_node_rgb(config->theme, "common.scrollbar_track_color");
    serverlist->thumb_color = get_node_rgb(config->theme, "common.scrollbar_thumb_color");

    messages->bg_color = get_node_rgb(config->theme, "common.primary_color");
    messages->button_color = get_node_rgb(config->theme, "common.scrollbar_button_color");
    messages->track_color = get_node_rgb(config->theme, "common.scrollbar_track_color");
    messages->thumb_color = get_node_rgb(config->theme, "common.scrollbar_thumb_color");

    messagebox->submit = &message_submit;
    messagebox->on_keypress = &handle_mb_kp;

    messagebox->bg_color = get_node_rgb(config->theme, "common.tertiary_color");
    messagebox->border_color = get_node_rgb(config->theme, "common.tertiary_color");
    messagebox->text_color = get_node_rgb(config->theme, "common.text_color");

    main_window->handle_bg_tasks = &client_run_tasks;
    main_window->on_resized      = &client_recalculate_sizes;

    main_window->add_widget(strip);
    main_window->add_widget(serverlist);
    main_window->add_widget(messagebox);
    main_window->add_widget(messages);

    for (int i = 0; i < config->server_count; i++) {
        server_list_add_server(serverlist, config->servers[i]);
    }

    main_window->show(1);
    
    // cleanup
    delete main_window;

    for (int i = 0; i < server_count; i++) {
        free(servers[i]);
    }
    free(servers);

    client_config_free(config);
    theme_tree_fini();
}
