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

#ifndef IRCREBORN_COMMON_UTIL_H
#define IRCREBORN_COMMON_UTIL_H

#ifdef  _WIN32
#define WOE32
#endif 

#include <string.h>
#include <stdio.h>

// access specifiers should only be a suggestion.
// in the end it is the user's choice to use unstable parts of the library, not mine.
#define private public
#define protected public

#define SSTRLEN(a) (sizeof(a) - 1)
#define STREQ(a, b) (strcmp(a, b) == 0)
#define STRNE(a, b) (!(STREQ(a, b)))

#define PINFO(format, ...) printf(" == | " format, ##__VA_ARGS__)
#define PWARN(format, ...) printf(" !  | " format, ##__VA_ARGS__)
#define PFATL(format, ...) printf(" !! | " format, ##__VA_ARGS__) // i know its spelled fatal.
#define PCONT(format, ...) printf("    | " format, ##__VA_ARGS__)

char* format_last_error();
char* format_error(int errorcode);
void  debug_point();
int   check_pointer_valid(void* addr);

#endif
