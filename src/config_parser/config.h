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

#ifndef IRCREBORN_CONFIG_PARSER_H
#define IRCREBORN_CONFIG_PARSER_H

#define CONFIG_TYPE_INT    0x00
#define CONFIG_TYPE_STRING 0x01
#define CONFIG_TYPE_GROUP  0x02

typedef struct client_config_server client_config_server_t;
typedef struct client_config_file client_config_t;
typedef struct client_config_theme client_config_theme_t;
typedef struct server_config_file server_config_t;
typedef struct config_rgba config_rgba_t;

struct config_rgba {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct client_config_theme {
    config_rgba_t server_list_scrollbar_track_color;
    config_rgba_t server_list_scrollbar_thumb_color;
    config_rgba_t server_list_scrollbar_button_color;
    config_rgba_t server_list_bg_color;
    config_rgba_t server_list_item_bg_color;
    config_rgba_t server_list_item_text_color;
    config_rgba_t messages_scrollbar_track_color;
    config_rgba_t messages_scrollbar_thumb_color;
    config_rgba_t messages_scrollbar_button_color;
    config_rgba_t messages_bg_color;
    config_rgba_t message_bg_color;
    config_rgba_t message_author_bg_color;
    config_rgba_t message_author_text_color;
    config_rgba_t message_content_bg_color;
    config_rgba_t message_content_text_color;
    config_rgba_t toolbar_bg_color;
    config_rgba_t toolbar_item_bg_color;
    config_rgba_t toolbar_item_text_color;
    config_rgba_t toolbar_menu_bg_color;
    config_rgba_t toolbar_menu_item_bg_color;
    config_rgba_t toolbar_menu_item_text_color;
};

struct client_config_server {
    char* name;
    char* host;
    char* nick;
    int   port;
};

struct client_config_file {
    int                      nickname_width;
    client_config_theme_t    theme;
    int                      server_count;
    client_config_server_t** servers;
};

struct server_config_file {
    int                      listen_port;
};

client_config_t* cfgparser_parse_client_config(int fd);
server_config_t* cfgparser_parse_server_config(int fd);
void             client_config_free(client_config_t* config);
void             server_config_free(server_config_t* server);

#endif
