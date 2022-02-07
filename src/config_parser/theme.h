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

#ifndef IRCREBORN_CONFIG_THEME_H
#define IRCREBORN_CONFIG_THEME_H

#include <common/color.h>

#define NODE_TYPE_BRANCH 0x00
#define NODE_TYPE_RGBA   0x01

typedef struct client_config_theme_tree_node client_config_theme_tree_node_t;

struct client_config_theme_tree_node {
    char* name;
    int type;
    union {
        struct {
            int child_count;
            client_config_theme_tree_node_t** children;
        } branch;
        struct {
            rgba_t value;
        } rgba;
    } value;
};

void theme_tree_init();
char** split_theme_path(char* path);
client_config_theme_tree_node_t* get_theme_node(client_config_theme_tree_node_t* root, char** path);
void register_theme_node(char* path, int type);
void set_node_rgb(client_config_theme_tree_node_t* root, char* path, uint32_t value);
void set_node_default_rgb(char* path, uint32_t value);
client_config_theme_tree_node_t* duplicate_node(client_config_theme_tree_node_t* node);
rgba_t get_node_rgb(client_config_theme_tree_node_t* root, char* path);
void _print_theme(client_config_theme_tree_node_t* node, int depth);
void print_theme(client_config_theme_tree_node_t* root);
void free_node(client_config_theme_tree_node_t* node, int delete_orphans);
void theme_tree_fini();

extern client_config_theme_tree_node_t* base_tree;

#endif