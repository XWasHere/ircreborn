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

#ifndef NETWORKING_PACKET_H
#define NETWORKING_PACKET_H

typedef struct p_hello hello_t;
struct p_hello {
    // identity (server/client type)
    int   has_ident;
    char* ident;
};

typedef struct p_message message_t;
struct p_message {
    char* message;
    char* name;
};

typedef struct p_set_nickname set_nickname_t;
struct p_set_nickname {
    char* nickname;
};

void send_hello(int fd, hello_t* packet);
void send_message(int fd, message_t* packet);
void send_set_nickname(int fd, set_nickname_t* packet);

#endif