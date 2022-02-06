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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <common/util.h>

#define INIT_STRING(name) name = malloc(1); name[0] = 0;

char* args_exec_name;
int   args_is_server;
char* args_config_path;
int   args_listen_port;
int   args_test;
int   args_verbose;

void parse_args(int argc, char** argv) {
    args_exec_name = argv[0];
    args_config_path = 0;
    args_listen_port = 0;
    args_verbose = 0;

#define next goto common_args_next;

    for (int i = 1; i < argc; i++) {
        if (STREQ(argv[i], "--server")) {
            args_is_server = 1;
            next;
        } else if (STREQ(argv[i], "--config")) {
            args_config_path = argv[i + 1];
            i += 1;
            next;
        } else if (STREQ(argv[i], "--listen-port")) {
            sscanf(argv[i+1], "%i", &args_listen_port);
            i += 1;
            next;
        } else if (STREQ(argv[i], "--test")) {
            args_test = 1;
            next;
        } else if (STREQ(argv[i], "--verbose")) {
            args_verbose++;
            next;
        }

        common_args_next:;
    }

#undef next
}
