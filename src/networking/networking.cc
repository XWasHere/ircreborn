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

#include <networking/opcodes.h>
#include <networking/packet.h>
#include <networking/types.h>
#include <common/util.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

void send_hello(int sock, hello_t* packet) {
    int   olen = 8 + 1 + 4 + strlen(packet->ident);
    char* obuf = (char*)malloc(olen);

    write_int    (obuf,     OPCODE_HELLO);
    write_int    (obuf + 4, olen - 8);    
    write_bool   (obuf + 8, packet->has_ident);
    write_string (obuf + 9, packet->ident, strlen(packet->ident));

    send(sock, obuf, olen, 0);

    free(obuf);
}

void send_message(int sock, message_t* packet) {
    int   olen = 8 + 4 + strlen(packet->message) + 4 + strlen(packet->name);
    char* obuf = (char*)malloc(olen);

    write_int    (obuf,     OPCODE_MESSAGE);
    write_int    (obuf + 4, olen - 8);
    write_string (obuf + 8, packet->message, strlen(packet->message));
    write_string (obuf + 8 + 4 + strlen(packet->message), packet->name, strlen(packet->name));

    send(sock, obuf, olen, 0);

    free(obuf);
}

void send_set_nickname(int sock, set_nickname_t* packet) {
    int olen = 8 + 4 + strlen(packet->nickname);
    char* obuf = (char*)malloc(olen);

    write_int    (obuf,     OPCODE_SET_NICKNAME);
    write_int    (obuf + 4, olen - 8);
    write_string (obuf + 8, packet->nickname, strlen(packet->nickname));
    send(sock, obuf, olen, 0);

    free(obuf);
}