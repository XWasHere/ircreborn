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

#include <stdlib.h>
#include <networking/types.h>
#include <stdint.h>
#include <string.h>

void write_int(void* buf, int i) {
    uint8_t* u8buf = buf;

    u8buf[0] = (i & 0x000000FF);
    u8buf[1] = (i & 0x0000FF00) >> 8;
    u8buf[2] = (i & 0x00FF0000) >> 16;
    u8buf[3] = (i & 0xFF000000) >> 24;
}

int read_int(void* buf) {
    uint8_t* u8buf = buf;

    return u8buf[0] | u8buf[1] << 8 | u8buf[2] << 16 | u8buf[3] << 24;
}

void write_string(void* buf, char* str, int len) {
    write_int(buf, len);
    memcpy(buf + 4, str, len);
}

nstring_t* read_string(void* buf) {
    nstring_t* s = malloc(sizeof(nstring_t));
    
    s->len = read_int(buf);
    s->str = malloc(s->len + 1);

    memset(s->str, 0, s->len + 1);
    memcpy(s->str, buf + 4, s->len);

    return s;
}

void write_bool(void* buf, int v) {
    uint8_t* u8buf = buf;
    
    if (v) u8buf[0] = 1;
    else   u8buf[0] = 0;
}

int read_bool(void* buf) {
    uint8_t* u8buf = buf;

    if (u8buf[0]) return 1;
    else          return 0;
}
