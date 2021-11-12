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

typedef struct config_server server_t;
struct config_server {
    char* name;
    char* host;
    int   port;
};

typedef struct config_file config_t;
struct config_file {
    int        server_count;
    server_t** servers;
};

config_t* parse_config(int fd);

#endif
