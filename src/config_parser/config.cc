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

#include <main.h>
#include <config_parser/config.h>
#include <common/util.h>
#include <compat/compat.h>
#include <common/logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int parse_pos;
static int parse_fd;
static int line;
static int col;

static char get_char(int consume) {
    char data;
    if (pread(parse_fd, &data, 1, parse_pos) == 0) return -1;
    if (consume) {
        parse_pos++;
        col++;
        if (data == '\n') {
            col = 1;
            line++;
        }
    }
    return data;
}

static int test_char(char c, int consume) {
    if (get_char(0) == c) {
        if (consume) get_char(1);
        return 1;
    } else return 0;
}

static char* get_token(int consume) {
    int bt = parse_pos;
    char* token = (char*)malloc(1);
    token[0] = 0;

    while (1) {
        if      (test_char(' ', 0))  break;
        else if (test_char('\t', 0)) break;
        else if (test_char('\n', 0)) break;
        char c = get_char(0);
        parse_pos++;
        token = (char*)realloc(token, strlen(token) + 2);
        token[strlen(token) + 1] = 0;
        token[strlen(token)] = c;
    }

    parse_pos = bt;

    if (consume) {
        for (int i = 0; i < strlen(token); i++) get_char(1);
    }

    return token;
}

static int test_str(char* str, int consume) {
    int l = strlen(str);
    char* data = (char*)malloc(l + 1);
    memset(data, 0, l + 1);

    pread(parse_fd, data, l, parse_pos);

    if (STREQ(str, data)) {
        for (int i = 0; i < l; i++) get_char(1);
        free(data);
        return 1;
    } else {
        free(data);
        return 0;
    }
}

static void consume_whitespace() {
    while (1) {
        if (test_char('\t', 1)) {}
        else if (test_char(' ', 1)) {}
        else if (test_char('\n', 1)) {}
        else if (test_char('\r', 1)) {}
        else break;
    }
}

static int is_done() {
    char junk;
    if (pread(parse_fd, &junk, 1, parse_pos) != 1) return 1;
    else return 0;
}

static char* read_string() {
    char* str = (char*)malloc(1);
    int   l   = 0;

    if (test_char('"', 1)) {
        while (!test_char('"', 0)) {
            l++;
            str = (char*)realloc(str, l + 1);
            str[l] = 0;
            str[l-1] = get_char(1);
        }

        get_char(1);
        return str;
    }

    free(str);
    return (char*)-1;
}

static int read_int() {
    char* buf = (char*)malloc(1);
    int   l   = 0;
    int   res = 0;

    while (1) {
        char c = get_char(0);
        if (c >= '0' && c <= '9') {
            l++;
            buf = (char*)realloc(buf, l + 1);
            buf[l] = 0; // this used to be buf[c] = 0. i need more sleep
            buf[l-1] = get_char(1);
        } else break;
    }

    sscanf(buf, "%i", &res);
    free(buf);

    return res;
}

static rgba_t read_rgba() {
    rgba_t* hi = (rgba_t*)malloc(sizeof(rgba_t));
    char* buf = (char*)malloc(10);
    memset(buf, 0, 10);
    pread(parse_fd, buf, 9, parse_pos);
    parse_pos += sizeof("#RRGGBBAA") - 1;
    sscanf(buf, "#%02x%02x%02x%02x", &hi->r, &hi->g, &hi->b, &hi->a);
    return *hi;
}

static void config_error(int l, int c, char* msg) {
    logger->log(CHANNEL_FATL, "error on line %i, col %i: %s\n", l, c, msg);
    exit(1);
}

static void die(char* error) {
    config_error(line, col, error);
}

#ifdef PARSER_LOG_SPAM
#define PARSER_LOG(a, b, ...) logger->log(a, b, ##__VA_ARGS__)
#else
#define PARSER_LOG(a, b, ...)
#endif

client_config_t* cfgparser_parse_client_config(int fd) {
    client_config_t *config = (client_config_t*)malloc(sizeof(client_config_t));
    
    config->theme = duplicate_node(base_tree);

    config->server_count = 0;
    config->servers = (client_config_server_t**) malloc(1);
    config->nickname_width = 320;
    
    parse_pos = 0;
    parse_fd  = fd;
    line      = 1;
    col       = 1;

    PARSER_LOG(CHANNEL_DBUG, "begin parsing config\n");
    while (!is_done()) {
        PARSER_LOG(CHANNEL_DBUG, "  parser cycle begin\n");
        consume_whitespace();
        if (test_str("server", 1)) {
            PARSER_LOG(CHANNEL_DBUG, "    begin parsing server section\n");
            consume_whitespace();
            client_config_server_t* server = (client_config_server_t*)malloc(sizeof(client_config_server_t));
            server->name = read_string();
            server->nick = 0;
            consume_whitespace();
            if (test_char('{', 1)) {
                PARSER_LOG(CHANNEL_DBUG, "      begin parsing server \"%s\"\n", server->name);
                while (1) {
                    consume_whitespace();
                    if (test_str("host", 1)) {
                        consume_whitespace();
                        server->host = read_string();
                        PARSER_LOG(CHANNEL_DBUG, "        with hostname \"%s\"\n", server->host);
                    } else if (test_str("port", 1)) {
                        consume_whitespace();
                        server->port = read_int();
                        PARSER_LOG(CHANNEL_DBUG, "        with port %i\n", server->port);
                    } else if (test_str("nick", 1)) {
                        consume_whitespace();
                        server->nick = read_string();
                        PARSER_LOG(CHANNEL_DBUG, "        with nickname \"%s\"\n", server->nick);
                    } else if (test_char('}', 1)) {
                        config->server_count++;
                        config->servers = (client_config_server_t**)realloc(config->servers, config->server_count * sizeof(void*));
                        config->servers[config->server_count - 1] = server;
                        logger->log(CHANNEL_DBUG, "registered server \"%s:%i\" as \"%s\"\n", server->host, server->port, server->name);
                        break;
                    } else {
                        char* error = (char*)malloc(255);
                        sprintf(error, "unknown token \"%s\"", get_token(0));
                        die(error);
                    }
                }
            } else {
                die("expected {");
            }
        } else if (test_str("nick_width", 1)) {
            consume_whitespace();
            config->nickname_width = read_int();
            PARSER_LOG(CHANNEL_DBUG, "    nickname width is %i\n", config->nickname_width);
        } else if (test_str("theme", 1)) {
            PARSER_LOG(CHANNEL_DBUG, "    begin parsing theme\n");
            consume_whitespace();
            if (test_char('{', 1)) {
                PARSER_LOG(CHANNEL_DBUG, "      enter theme block\n");
                consume_whitespace();
                char** tree = (char**)malloc(sizeof(char*) * 64);
                int    tree_pos = 0;
                int    depth = 1;
                memset(tree, 0, sizeof(void*) * 64);
                PARSER_LOG(CHANNEL_DBUG, "        depth is %i\n", depth);

                while (depth > 0) {
                    PARSER_LOG(CHANNEL_DBUG, "        begin theme parse cycle\n");
                    consume_whitespace();
                    if (test_char('}', 1)) {
                        depth--;
                        tree_pos--;
                        PARSER_LOG(CHANNEL_DBUG, "          depth is %i\n", depth);
                    } else if (test_char('{', 1)) {
                        depth++;
                        PARSER_LOG(CHANNEL_DBUG, "          depth is %i\n", depth);
                    } else {
                        client_config_theme_tree_node_t *node = config->theme;
                        char* name = get_token(1);
                        PARSER_LOG(CHANNEL_DBUG, "          begin parsing for node \"%s\"\n", name);
                        tree[tree_pos] = name;
                        tree_pos++;
                        PARSER_LOG(CHANNEL_DBUG, "            tree pos is %i\n", tree_pos);
                        PARSER_LOG(CHANNEL_DBUG, "            finding node \"%s\"\n", name);
                        for (int i = 0; i < tree_pos; i++) {
                            if (node->type == NODE_TYPE_BRANCH) {
                                for (int ii = 0; ii < node->value.branch.child_count; ii++) {
                                    PARSER_LOG(CHANNEL_DBUG, "              considering node \"%s\"\n", node->value.branch.children[ii]->name);
                                    if (strcmp(node->value.branch.children[ii]->name, tree[i]) == 0) {
                                        node = node->value.branch.children[ii];
                                        PARSER_LOG(CHANNEL_DBUG, "                selected node \"%s\"\n", node->name);
                                        break;
                                    } else {
                                        PARSER_LOG(CHANNEL_DBUG, "                rejected node \"%s\"\n", node->value.branch.children[ii]->name);
                                    }
                                }
                            }
                        }
                        
                        if (node->type == NODE_TYPE_BRANCH) {
                            PARSER_LOG(CHANNEL_DBUG, "            type is branch\n");
                        } else if (node->type == NODE_TYPE_RGBA) {
                            consume_whitespace();
                            PARSER_LOG(CHANNEL_DBUG, "            type is rgba\n");
                            PARSER_LOG(CHANNEL_DBUG, "            set value of node \"%s\"\n", node->name);
                            rgba_t val = node->value.rgba.value = read_rgba();
                            PARSER_LOG(CHANNEL_DBUG, "              new value is #%02x%02x%02x%02x\n", val.r, val.g, val.b, val.a);
                            tree_pos--;
                        }
                    }
                }
                
                free(tree);
            } else {
                die("expected {");
            }
        } else {
            char* error = (char*)malloc(255);
            sprintf(error, "unknown token \"%s\"", get_token(0));
            die(error);
        }

        consume_whitespace();
    }

#ifdef PARSER_LOG_SPAM
    logger->log(CHANNEL_DBUG, "USING THEME\n");
    print_theme(config->theme);
#endif

    return config;
}

server_config_t* cfgparser_parse_server_config(int fd) {
    server_config_t* config = (server_config_t*)malloc(sizeof(server_config_t));

    config->listen_port = 10010;

    parse_pos = 0;
    parse_fd  = fd;

    while (!is_done()) {
        consume_whitespace();
        if (test_str("listen", 1)) {
            consume_whitespace();
            config->listen_port = read_int();
        } else if (test_str("default_nick", 1)) {
            if (test_str("string",1)) {
                
            }
        }
    }

    return config;
}

void client_config_free(client_config_t* config) {
    for (int i = 0; i < config->server_count; i++) {
        if (config->servers[i]->host) {
            free(config->servers[i]->host);
        }
        if (config->servers[i]->name) {
            free(config->servers[i]->name);
        }
        if (config->servers[i]->nick) {
            free(config->servers[i]->nick);
        }
        free(config->servers[i]);
    }
    free(config->servers);
    free_node(config->theme, 1);
    free(config);
}