#include <stdlib.h>

#include <common/logger.h>
#include <config_parser/theme.h>

client_config_theme_tree_node_t* base_tree;

void theme_tree_init() {
    base_tree = malloc(sizeof(client_config_theme_tree_node_t));
}

char** split_theme_path(char* path) {
    int nodes = 1;
    int pos = 0;

    while (path[pos] != 0) {
        if (path[pos] == '.') nodes++;
        pos++;
    }

    char** opath = malloc(sizeof(char*) * (nodes + 1));
    opath[nodes] = 0;

    int node = 0;
    int len = 0;
    pos = 0;

    for (int i = 0; i < nodes; i++) {
        opath[i] = malloc(1);
    }

    while (path[pos] != 0) {
        if (path[pos] == '.') {
            node++;
            len = 0;
            pos++;
        }
        len++;
        opath[node] = realloc(opath[node], len + 1);
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

    client_config_theme_tree_node_t* node = malloc(sizeof(client_config_theme_tree_node_t));
    node->name = temp;
    node->type = type;
    if (node->type == NODE_TYPE_BRANCH) {
        node->value.branch.child_count = 0;
        node->value.branch.children = malloc(1);
    } else if (node->type == NODE_TYPE_RGBA) {
        node->value.rgba.value.r = 0;
        node->value.rgba.value.g = 0;
        node->value.rgba.value.b = 0;
        node->value.rgba.value.a = 0;
    }

    if (temp == 0) {
        base_tree->value.branch.child_count++;
        base_tree->value.branch.children = realloc(base_tree->value.branch.children, sizeof(void*) * base_tree->value.branch.child_count);
        base_tree->value.branch.children[base_tree->value.branch.child_count - 1] = node;
    } else {
        client_config_theme_tree_node_t* parent = get_theme_node(base_tree, p);
        parent->value.branch.child_count++;
        parent->value.branch.children = realloc(parent->value.branch.children, sizeof(void*) * parent->value.branch.child_count);
        parent->value.branch.children[parent->value.branch.child_count - 1] = node;
    }
}

void set_node_rgb(client_config_theme_tree_node_t* root, char* path, uint32_t value) {
    client_config_theme_tree_node_t* node = get_theme_node(root, split_theme_path(path));
    printf("%s\n", node->name);
    memcpy(&node->value.rgba.value, &value, sizeof(uint32_t));
}

void set_node_default_rgb(char* path, uint32_t value) {
    set_node_rgb(base_tree, path, value);
}

client_config_theme_tree_node_t* duplicate_node(client_config_theme_tree_node_t* node) {
    client_config_theme_tree_node_t* out = malloc(sizeof(client_config_theme_tree_node_t));

    if (node->name) {
        out->name = malloc(strlen(node->name) + 1);
        memset(out->name, 0, strlen(node->name) + 1);
        strcpy(out->name, node->name);
    }
    out->type = node->type;
    
    if (out->type == NODE_TYPE_BRANCH) {
        out->value.branch.child_count = node->value.branch.child_count;
        out->value.branch.children = malloc(sizeof(void*) * out->value.branch.child_count);
        for (int i = 0; i < out->value.branch.child_count; i++) {
            out->value.branch.children[i] = duplicate_node(node->value.branch.children[i]);
        }
    } else if (out->type == NODE_TYPE_RGBA) {
        memcpy(&out->value.rgba.value, &node->value.rgba.value, sizeof(uint32_t));
    }

    return out;
}

rgba_t get_node_rgb(client_config_theme_tree_node_t* root, char* path) {
    client_config_theme_tree_node_t* out = get_theme_node(root, split_theme_path(path));
    return out->value.rgba.value;
}

void _print_theme(client_config_theme_tree_node_t* node, int depth) {
    if (node->type == NODE_TYPE_BRANCH) {
        logger_log(CHANNEL_DBUG, "% *sBRANCH \"%s\" LENGTH %i\n", depth, "", node->name == 0 ? "<ROOT>" : node->name, node->value.branch.child_count);
        for (int i = 0; i < node->value.branch.child_count; i++) {
            _print_theme(node->value.branch.children[i], depth+1);
        }
    } else if (node->type == NODE_TYPE_RGBA) {
        logger_log(CHANNEL_DBUG, "% *sRGBA \"%s\" #%02x%02x%02x%02x\n", depth, "", node->name, node->value.rgba.value.r, node->value.rgba.value.g, node->value.rgba.value.b, node->value.rgba.value.a);
    }
}

void print_theme(client_config_theme_tree_node_t* root) {
    _print_theme(root, 0);
}