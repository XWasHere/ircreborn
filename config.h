#ifndef IRCREBORN_CONFIG_PARSER_H
#define IRCREBORN_CONFIG_PARSER_H

#define CONFIG_TYPE_INT    0x00
#define CONFIG_TYPE_STRING 0x01
#define CONFIG_TYPE_GROUP  0x02

typedef struct config_group   config_group_t;
typedef struct config_value   config_value_t;
typedef        config_group_t config_t;

struct config_group {
    char*           group_type;

    int             has_name;
    char*           name;
    
    int             value_count;
    config_value_t* values;
};

struct config_value {
    char* name;

    int   value_type;

    union values {
        char*           stringv;
        int             intv;
        config_group_t* groupv;
    };
};

config_t* parse_config(int fd);

#endif