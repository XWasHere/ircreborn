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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <common/args.h>
#include <common/util.h>
#include <networking/networking.h>
#include <networking/types.h>
#include <config_parser/config.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#endif

struct client {
    int fd;

    int has_nickname;
    char* nickname;
};

int             client_count = 0;
struct client** clients;

struct client* find_client(int fd) {
    for (int i = 0; i < client_count) {
        if (clients[i]->fd == fd) return clients[i];
    }
    return -1;
}

void server_main() {
#ifdef WIN32
    WSADATA* wsadata = malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        printf(FMT_FATL("failed to start winsock, aborting\n"));
        printf(FMT_FATL("%s"), format_error(WSAGetLastError()));
        exit(1);
    }
#endif
    char* config_path = args_config_path;
    if (config_path == 0) {
        config_path = malloc(255);
        memset(config_path, 0, 255);
#ifdef WIN32
        strcat(config_path, getenv("USERPROFILE"));
#else
        strcat(config_path, getenv("HOME"));
#endif
        strcat(config_path, "/.ircreborn/server");
    }

    printf(FMT_INFO("reading config from %s\n"), config_path);

    int configfd = open(config_path, O_RDONLY | O_CREAT);
    chmod(config_path, S_IWUSR | S_IRUSR);
    server_config_t* config = cfgparser_parse_server_config(configfd);
    close(configfd);

    int one = 1;

    int server;
    struct sockaddr_in* server_addr = malloc(sizeof(struct sockaddr_in));

    server = socket(AF_INET, SOCK_STREAM, 0);

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(args_listen_port ? args_listen_port : config->listen_port);

#ifdef WIN32
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one));
#else
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#endif

    bind(server, (struct sockaddr*)server_addr, sizeof(struct sockaddr_in));

    listen(server, 3);

    int             pollfd_count = 1;
    struct pollfd*  pollfds      = malloc(sizeof(struct pollfd));
    clients                      = malloc(1);


    pollfds[0].fd = server;
    pollfds[0].events = POLLIN;

    while (1) {
#ifdef WIN32
        WSAPoll(pollfds, pollfd_count, -1);
#else
        poll(pollfds, pollfd_count, -1);
#endif
        if (pollfds[0].revents & POLLIN) {
            struct sockaddr_in* addr     = malloc(sizeof(struct sockaddr_in));
            int*                addr_len = malloc(4);
            addr_len[0]                  = sizeof(struct sockaddr_in);

            int fd = accept(server, (struct sockaddr*)addr, addr_len);

            printf(FMT_INFO("accepted\n"));

            pollfd_count++;
            pollfds = realloc(pollfds, pollfd_count * sizeof(struct pollfd));
            pollfds[pollfd_count - 1].fd = fd;
            pollfds[pollfd_count - 1].events = POLLIN;
        }

        for (int i = 1; i < pollfd_count; i++) {
            if (pollfds[i].revents & POLLERR) {
                printf(FMT_INFO("error\n"));
                goto pollhup_handler;
            } else if (pollfds[i].revents & POLLHUP) {
                pollhup_handler:;
                for (int ii = i; ii < pollfd_count - 1; ii++) {
                    pollfds[ii] = pollfds[ii + 1];
                }
                pollfd_count--;
            } else if (pollfds[i].revents & POLLIN) {
                char c;
                char* msgbuf = 0;
                int   bufpos = 0;

                void* header = malloc(8);
                int   buflen = 0;
                int   op     = 0;

                if (recv(pollfds[i].fd, header, 8, 0) == 0) {
                    printf(FMT_WARN("got 0 bytes of data. assuming broken connection. kicking\n"));
                    close(pollfds[i].fd);
                    goto pollhup_handler;
                } else {
                    op     = read_int(header);
                    buflen = read_int(header + 4);
                    
                    msgbuf = malloc(buflen + 1);
                    memset(msgbuf, 0, buflen + 1);

                    int res = recv(pollfds[i].fd, msgbuf, buflen, 0);

                    if (op == OPCODE_HELLO) {
                        int has_ident    = read_bool(msgbuf);
                        nstring_t* ident = read_string(msgbuf + 1);
                        client_count++;
                        clients = realloc(clients, sizeof(void*) * client_count);
                        struct client* c = malloc(sizeof(struct client));
                        clients[client_count - 1] = c;
                        c->nickname = malloc(5);
                        
                        strcpy(c->nickname, "anon");
                        c->fd = pollfds[i].fd;
                        set_nickname_t* snpacket = malloc(sizeof(set_nickname_t));
                        snpacket->nickname = "anon";
                        send_set_nickname(c->fd, snpacket);
                        free(snpacket);
                    } else if (op == OPCODE_MESSAGE) {
                        nstring_t* imsg = read_string(msgbuf);
                        message_t* omsg = malloc(sizeof(message_t));

                        omsg->message = imsg->str;

                        // replicate
                        for (int i = 1; i < pollfd_count; i++) {
                            send_message(pollfds[i].fd, omsg);
                        }
                    } else if (op == OPCODE_SET_NICKNAME) {
                        
                    }

                    free(msgbuf);
                    free(header);
                    fflush(0);
                }
            }
        }
    }
}