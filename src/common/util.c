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

#include <common/util.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32 
#include <windows.h> // yayyyyyyyyyyyy
#endif

void gdb_breakpoint() {};

char* format_last_error() {
    int errorcode;
#ifdef WIN32
    errorcode = GetLastError();
#endif
    return format_error(errorcode);
}

char* format_error(int errorcode) {
    char* error;
    char* errordesc;
    
#ifdef WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types" // nobody asked
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        errorcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        &errordesc,
        0,
        0
    );
#pragma GCC diagnostic pop
#endif

    error = malloc(strlen(errordesc) + 14);
    memset(error, 0, strlen(errordesc) + 14);
    sprintf(error, "0x%08x - %s", errorcode, errordesc);

    return error;
}
