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

extern client_config_theme_tree_node_t* base_tree;

#endif