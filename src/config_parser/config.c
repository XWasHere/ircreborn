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

#include <config_parser/config.h>
#include <common/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define get_char            cfgparser_get_char
#define test_char           cfgparser_test_char
#define test_str            cfgparser_test_str
#define consume_whitespace  cfgparser_consume_whitespace
#define is_done             cfgparser_is_done
#define read_string         cfgparser_read_string
#define read_int            cfgparser_read_int
#define parse_client_config cfgparser_parse_client_config
#define parse_server_config cfgparser_parse_server_config
#define parse_pos           cfgparser_parse_pos
#define parse_fd            cfgparser_parse_fd

int parse_pos;
int parse_fd;

char get_char(int consume) {
    char data;
    if (pread(parse_fd, &data, 1, parse_pos) == 0) return -1;
    if (consume) parse_pos++;
    return data;
}

int test_char(char c, int consume) {
    if (get_char(0) == c) {
        if (consume) parse_pos++;
        return 1;
    } else return 0;
}

int test_str(char* str, int consume) {
    int l = strlen(str);
    char* data = malloc(l + 1);
    memset(data, 0, l + 1);

    pread(parse_fd, data, l, parse_pos);

    if (STREQ(str, data)) {
        if (consume) parse_pos += l;
        return 1;
    } else return 0;
}

void consume_whitespace() {
    while (1) {
        if (test_char('\t', 1)) {}
        else if (test_char(' ', 1)) {}
        else if (test_char('\n', 1)) {}
        else if (test_char('\r', 1)) {}
        else break;
    }
}

int is_done() {
    char junk;
    if (pread(parse_fd, &junk, 1, parse_pos) != 1) return 1;
    else return 0;
}

char* read_string() {
    char* str = malloc(1);
    int   l   = 0;

    if (test_char('"', 1)) {
        while (!test_char('"', 0)) {
            l++;
            str = realloc(str, l + 1);
            str[l] = 0;
            str[l-1] = get_char(1);
        }

        get_char(1);

        return str;
    }

    free(str);
    return -1;
}

int read_int() {
    char* buf = malloc(1);
    int   l   = 0;
    int   res = 0;

    while (1) {
        char c = get_char(0);
        if (c >= '0' && c <= '9') {
            l++;
            buf = realloc(buf, l + 1);
            buf[c] = 0;
            buf[l-1] = get_char(1);
        } else break;
    }

    sscanf(buf, "%i", &res);
    free(buf);

    return res;
}

client_config_t* parse_client_config(int fd) {
    client_config_t *config = malloc(sizeof(client_config_t));
    
    config->server_count = 0;
    config->servers = malloc(1);
    
    parse_pos = 0;
    parse_fd  = fd;

    while (!is_done()) {
        consume_whitespace();
        if (test_str("server", 1)) {
            consume_whitespace();
            client_config_server_t* server = malloc(sizeof(client_config_server_t));
            server->name = read_string();
            consume_whitespace();
            if (test_char('{', 1)) {
                while (1) {
                    consume_whitespace();
                    if (test_str("host", 1)) {
                        consume_whitespace();
                        server->host = read_string();
                    } else if (test_str("port", 1)) {
                        consume_whitespace();
                        server->port = read_int();
                    } else if (test_char('}', 1)) {
                        config->server_count++;
                        config->servers = realloc(config->servers, config->server_count * sizeof(void*));
                        config->servers[config->server_count - 1] = server;
                        printf(FMT_INFO("registered server \"%s:%i\" as \"%s\"\n"), server->host, server->port, server->name);
                        break;
                    } else {
                        printf(FMT_FATL("invalid config\n"));
                        exit(1);
                    }
                }
            }
        }
    }

    return config;
}

server_config_t* parse_server_config(int fd) {
    server_config_t* config = malloc(sizeof(server_config_t));

    config->listen_port = 10010;

    parse_pos = 0;
    parse_fd  = fd;

    while (!is_done()) {
        consume_whitespace();
        if (test_str("listen", 1)) {
            consume_whitespace();
            config->listen_port = read_int();
        }
    }

    return config;
}