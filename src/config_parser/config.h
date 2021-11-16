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
typedef struct server_config_file server_config_t;

struct client_config_server {
    char* name;
    char* host;
    int   port;
};

struct client_config_file {
    int                      server_count;
    client_config_server_t** servers;
};

struct server_config_file {
    int                      listen_port;
};

client_config_t* cfgparser_parse_client_config(int fd);
server_config_t* cfgparser_parse_server_config(int fd);

#endif
