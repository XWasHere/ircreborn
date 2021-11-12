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

#ifndef NETWORKING_TYPES_H
#define NETWORKING_TYPES_H

typedef struct network_string nstring_t;
struct network_string {
    int   len;
    char* str;
};

void       write_string(void* buf, char* str, int len);
nstring_t* read_string(void* buf);
void       write_int(void* buf, int i);
int        read_int(void* buf);
void       write_bool(void* buf, int v);
int        read_bool(void* buf);

#endif
