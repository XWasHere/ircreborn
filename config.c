#include <config_parser/config.h>
#include <common/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

config_t* parse_config(int fd) {
    config_group_t *config = malloc(sizeof(config_group_t));

    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == 's') {
            read(fd, &c, 1);
            if (c == 'e') {
                read(fd, &c, 1);
                if (c == 'r') {
                    read(fd, &c, 1);
                    if (c == 'v') {
                        read(fd, &c, 1);
                        if (c == 'e') {
                            read(fd, &c, 1);
                            if (c == 'r') {
                                printf("server\n");
                                read(fd, &c, 1);
                                while (c == ' ')
                            }
                        }
                    }
                }
            }
        }
    }

    return config;
}