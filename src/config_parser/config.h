#ifndef IRCREBORN_CONFIG_PARSER_H
#define IRCREBORN_CONFIG_PARSER_H

#define CONFIG_TYPE_INT    0x00
#define CONFIG_TYPE_STRING 0x01
#define CONFIG_TYPE_GROUP  0x02

typedef struct config_server server_t;
struct config_server {
    char* name;
    char* host;
    int   port;
};

typedef struct config_file config_t;
struct config_file {
    int        server_count;
    server_t** servers;
};

config_t* parse_config(int fd);

#endif
