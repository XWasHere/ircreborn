#include <config_parser/config.h>
#include <common/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

config_t* parse_config(int fd) {
    config_t *config = malloc(sizeof(config_t));
    
    config->server_count = 0;
    config->servers = malloc(1);
    
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
                                server_t* server = malloc(sizeof(server_t));

                                server->name = malloc(1);
                                int nl = 0;

                                read(fd, &c, 1);
                                while (c == ' ') { read(fd, &c, 1); }
                                read(fd, &c, 1);
                                
                                while (c != '"') {
                                    nl++;
                                    server->name = realloc(server->name, nl + 1);
                                    server->name[nl] = 0;
                                    server->name[nl-1] = c;
                                    read(fd, &c, 1);
                                }

                                read(fd, &c, 1);
                                while (c == ' ') { read(fd, &c, 1); }

                                if (c == '{') {
                                    while (1) {
                                        while (1) {
                                            read(fd, &c, 1);
                                            if (!(c == '\t' || c == ' ' || c == '\n')) {
                                                break;
                                            }
                                        }

                                        if (c == 'h') {
                                            read(fd, &c, 1);
                                            if (c == 'o') {
                                                read(fd, &c, 1);
                                                if (c == 's') {
                                                    read(fd, &c, 1);
                                                    if (c == 't') {
                                                        read(fd, &c, 1);
                                                        while (c == ' ') { read(fd, &c, 1); }
                                                        read(fd, &c, 1);

                                                        nl = 0;
                                                        server->host = malloc(1);

                                                        while (c != '"') {
                                                            nl++;
                                                            server->host = realloc(server->host, nl + 1);
                                                            server->host[nl] = 0;
                                                            server->host[nl-1] = c;
                                                            read(fd, &c, 1);
                                                        }
                                                    }
                                                }
                                            }
                                        } else if (c == 'p') {
                                            read(fd, &c, 1);
                                            if (c == 'o') {
                                                read(fd, &c, 1);
                                                if (c == 'r') {
                                                    read(fd, &c, 1);
                                                    if (c == 't') {
                                                        read(fd, &c, 1);
                                                        while (c == ' ') { read(fd, &c, 1); }

                                                        char* p = malloc(1);

                                                        nl = 0;
                                                        while (c >= '0' && c <= '9') {
                                                            nl++;
                                                            p = realloc(p, nl + 1);
                                                            p[nl] = 0;
                                                            p[nl-1] = c;
                                                            read(fd, &c, 1);
                                                        }

                                                        sscanf(p, "%i", &server->port);
                                                    }
                                                }
                                            }
                                        } else if (c == '}') {
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
                    }
                }
            }
        }
    }

    return config;
}
