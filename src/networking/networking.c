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

#include <networking/opcodes.h>
#include <networking/packet.h>
#include <networking/types.h>
#include <common/util.h>

#include <unistd.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

void send_hello(int sock, hello_t* packet) {
    int   olen = 8 + 1 + 4 + strlen(packet->ident);
    char* obuf = malloc(olen);

    write_int    (obuf,     OPCODE_HELLO);
    write_int    (obuf + 4, olen - 8);    
    write_bool   (obuf + 8, packet->has_ident);
    write_string (obuf + 9, packet->ident, strlen(packet->ident));

    send(sock, obuf, olen, 0);
}

void send_message(int sock, message_t* packet) {
    int   olen = 8 + 4 + strlen(packet->message);
    char* obuf = malloc(olen);

    write_int    (obuf,     OPCODE_MESSAGE);
    write_int    (obuf + 4, olen - 8);
    write_string (obuf + 8, packet->message, strlen(packet->message));

    send(sock, obuf, olen, 0);
}