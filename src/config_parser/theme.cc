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

#include <stdlib.h>
#include <string.h>

#include <main.h>
#include <common/logger.h>
#include <config_parser/theme.h>

client_config_theme_tree_node_t* base_tree;

void theme_tree_init() {
    base_tree = (client_config_theme_tree_node_t*)malloc(sizeof(client_config_theme_tree_node_t));
    base_tree->name = 0;
    base_tree->type = NODE_TYPE_BRANCH;
    base_tree->value.branch.child_count = 0;
    base_tree->value.branch.children = (client_config_theme_tree_node_t**)malloc(sizeof(void*));
}

char** split_theme_path(char* path) {
    int nodes = 1;
    int pos = 0;

    while (path[pos] != 0) {
        if (path[pos] == '.') nodes++;
        pos++;
    }

    char** opath = (char**)malloc(sizeof(char*) * (nodes + 1));
    opath[nodes] = 0;

    int node = 0;
    int len = 0;
    pos = 0;

    for (int i = 0; i < nodes; i++) {
        opath[i] = (char*)malloc(1);
    }

    while (path[pos] != 0) {
        if (path[pos] == '.') {
            node++;
            len = 0;
            pos++;
        }
        len++;
        opath[node] = (char*)realloc(opath[node], len + 1);
        opath[node][len - 1] = path[pos];
        opath[node][len] = 0;
        pos++;
    }

    return opath;
}

client_config_theme_tree_node_t* get_theme_node(client_config_theme_tree_node_t* root, char** path) {
    client_config_theme_tree_node_t* node = root;
    int pos = 0;

    while (path[pos] != 0) {
        for (int i = 0; i < node->value.branch.child_count; i++) {
            if (strcmp(node->value.branch.children[i]->name, path[pos]) == 0) {
                node = node->value.branch.children[i];
                pos++;
                break;
            }
        }
    }

    return node;
}

void register_theme_node(char* path, int type) {
    char** p = split_theme_path(path);
    char* temp = 0;
    int   path_len = 0;

    for (int i = 0; p[i] != 0; i++) {
        path_len = i;
        temp = p[i];
    }

    p[path_len] = 0;

    client_config_theme_tree_node_t* node = (client_config_theme_tree_node_t*)malloc(sizeof(client_config_theme_tree_node_t));
    node->name = temp;
    node->type = type;
    if (node->type == NODE_TYPE_BRANCH) {
        node->value.branch.child_count = 0;
        node->value.branch.children = (client_config_theme_tree_node_t**)malloc(1);
    } else if (node->type == NODE_TYPE_RGBA) {
        node->value.rgba.value.r = 0;
        node->value.rgba.value.g = 0;
        node->value.rgba.value.b = 0;
        node->value.rgba.value.a = 0;
    }

    if (temp == 0) {
        base_tree->value.branch.child_count++;
        base_tree->value.branch.children = (client_config_theme_tree_node_t**)realloc(base_tree->value.branch.children, sizeof(void*) * base_tree->value.branch.child_count);
        base_tree->value.branch.children[base_tree->value.branch.child_count - 1] = node;
    } else {
        client_config_theme_tree_node_t* parent = get_theme_node(base_tree, p);
        parent->value.branch.child_count++;
        parent->value.branch.children = (client_config_theme_tree_node_t**)realloc(parent->value.branch.children, sizeof(void*) * parent->value.branch.child_count);
        parent->value.branch.children[parent->value.branch.child_count - 1] = node;
    }

    for (int i = 0; i < path_len; i++) {
        free(p[i]);
    }
    free(p);
}

void set_node_rgb(client_config_theme_tree_node_t* root, char* path, uint32_t value) {
    char** path2 = split_theme_path(path);

    client_config_theme_tree_node_t* node = get_theme_node(root, path2);
    memcpy(&node->value.rgba.value, &value, sizeof(uint32_t));

    for (int i = 0; path2[i] != 0; i++) {
        free(path2[i]);
    }
    free(path2);
}

void set_node_default_rgb(char* path, uint32_t value) {
    set_node_rgb(base_tree, path, value);
}

client_config_theme_tree_node_t* duplicate_node(client_config_theme_tree_node_t* node) {
    client_config_theme_tree_node_t* out = (client_config_theme_tree_node_t*)malloc(sizeof(client_config_theme_tree_node_t));

    if (node->name) {
        out->name = (char*)malloc(strlen(node->name) + 1);
        memset(out->name, 0, strlen(node->name) + 1);
        strcpy(out->name, node->name);
    }
    out->type = node->type;
    
    if (out->type == NODE_TYPE_BRANCH) {
        out->value.branch.child_count = node->value.branch.child_count;
        out->value.branch.children = (client_config_theme_tree_node_t**)malloc(sizeof(void*) * out->value.branch.child_count);
        for (int i = 0; i < out->value.branch.child_count; i++) {
            out->value.branch.children[i] = duplicate_node(node->value.branch.children[i]);
        }
    } else if (out->type == NODE_TYPE_RGBA) {
        memcpy(&out->value.rgba.value, &node->value.rgba.value, sizeof(uint32_t));
    }

    return out;
}

rgba_t get_node_rgb(client_config_theme_tree_node_t* root, char* path) {
    char** path2 = split_theme_path(path);
    client_config_theme_tree_node_t* out = get_theme_node(root, path2);
    
    for (int i = 0; path2[i] != 0; i++) {
        free(path2[i]);
    }
    free(path2);
    
    return out->value.rgba.value;
}

void free_node(client_config_theme_tree_node_t* node, int free_orphaned) {
    if (free_orphaned) {
        if (node->type == NODE_TYPE_BRANCH) {
            for (int i = 0; i < node->value.branch.child_count; i++) {
                free_node(node->value.branch.children[i], 1);
                node->value.branch.children[i] = 0;
            }
        }
    }

    if (node->type == NODE_TYPE_BRANCH) {
        free(node->value.branch.children);
    } else if (node->type == NODE_TYPE_RGBA) {
        
    }

    if (node->name) {
        free(node->name);
    }

    free(node);
}

void theme_tree_fini() {
    free_node(base_tree, 1);
}

void _print_theme(client_config_theme_tree_node_t* node, int depth) {
    if (node->type == NODE_TYPE_BRANCH) {
        logger->log(CHANNEL_DBUG, "% *sBRANCH \"%s\" LENGTH %i\n", depth, "", node->name == 0 ? "<ROOT>" : node->name, node->value.branch.child_count);
        for (int i = 0; i < node->value.branch.child_count; i++) {
            _print_theme(node->value.branch.children[i], depth+1);
        }
    } else if (node->type == NODE_TYPE_RGBA) {
        logger->log(CHANNEL_DBUG, "% *sRGBA \"%s\" #%02x%02x%02x%02x\n", depth, "", node->name, node->value.rgba.value.r, node->value.rgba.value.g, node->value.rgba.value.b, node->value.rgba.value.a);
    }
}

void print_theme(client_config_theme_tree_node_t* root) {
    _print_theme(root, 0);
}