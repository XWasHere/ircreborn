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

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#endif

#include <main.h>
#include <common/args.h>
#include <common/util.h>
#include <common/attrib.h>
#include <common/logger.h>
#include <networking/networking.h>
#include <config_parser/config.h>
#include <compat/compat.h>

struct client {
    ircreborn_connection_t* connection;
    
    int queue_gc_ticks;

    int state;

    int has_nickname;
    char* nickname;
};

int             pollfd_count = 0;
struct pollfd*  pollfds;
int             client_count = 0;
struct client** clients;

uint32_t protocol_count = 1;
uint32_t protocols[] = {
    1
};

int find_client_id(int fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i]->connection->fd == fd) return i;
    }
    return -1;
}

int find_client_id_from_client(struct client* client) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client) return i;
    }
    return -1;
}

struct client* find_client(int fd) {
    int i = find_client_id(fd);
    if (i != -1) {
        return clients[i];
    }
    return (struct client*)-1;
}

void send_client_message(struct client* client, char* msg, char* name) {
    ircreborn_precv_message_t packet;
    packet.message        = msg;
    packet.message_length = strlen(msg);
    packet.author         = name;
    packet.author_length  = strlen(name);
    client->connection->send_recv_message(&packet);
}

void send_all_message(char* msg, char* name) {
    for (int i = 0; i < client_count; i++) {
        // dont send messages to clients that arent ready
        if (clients[i]->state == 2) {
            send_client_message(clients[i], msg, name);
        }
    }
}

void disconnect_client(struct client* client, int send_message, int automatic, int has_reason, char* reason) {
    int cid = find_client_id_from_client(client);
    
    for (int i = cid; i < client_count - 1; i++) {
        clients[i] = clients[i + 1];
    }

    if (send_message) {
        int len = 1; // "
        len += strlen(client->nickname);
        len += 2; // " 
        len += automatic?SSTRLEN("auto-disconnected by server"):SSTRLEN("disconnected");
        len += has_reason?SSTRLEN(" [ ")+strlen(reason)+SSTRLEN(" ]"):0;
        len += 1;

        char* buf = malloc(len);
        memset(buf, 0, len);

        sprintf(
            buf,
            "\"%s\" %s%s%s%s", 
            client->nickname,
            automatic?"auto-disconnected by server":"disconnected",
            has_reason?" [ ":"",
            has_reason?reason:"",
            has_reason?" ]":"");
        
        send_all_message(buf, "==");
        free(buf);
    }

    client_count--;
}

void disconnect_socket(int fd, int send_message, int automatic, int has_reason, char* reason) {
    int cid = find_client_id(fd);
    if (cid != -1) {
        disconnect_client(clients[cid], send_message, automatic, has_reason, reason);
    }
    for (int i = fd; i < pollfd_count - 1; i++) {
        pollfds[i] = pollfds[i + 1];
    }
    pollfd_count--;
}

void set_nickname(struct client* c, char* nick) {
    int nlen = strlen(nick);

    c->nickname = malloc(nlen+1);
    memset(c->nickname, 0, nlen+1);
    memcpy(c->nickname, nick, nlen);

    ircreborn_pnickname_updated_t packet;
    packet.nickname_length = nlen;
    packet.nickname        = nick;
    c->connection->send_nickname_updated(&packet);
}

void server_main() {
#ifdef WIN32
    WSADATA* wsadata = (WSADATA*)malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        logger->log(CHANNEL_FATL, "failed to start winsock, aborting\n");
        logger->log(CHANNEL_FATL, "%s", format_error(WSAGetLastError()));
        exit(1);
    }
#endif
    char* config_path = args_config_path;
    if (config_path == 0) {
        config_path = (char*)malloc(255);
        memset(config_path, 0, 255);
#ifdef WIN32
        strcat(config_path, getenv("USERPROFILE"));
#else
        strcat(config_path, getenv("HOME"));
#endif
        strcat(config_path, "/.ircreborn/server");
    }

    logger->log(CHANNEL_INFO, "reading config from %s\n", config_path);

    int configfd = open(config_path, O_RDONLY | O_CREAT);
    chmod(config_path, S_IWUSR | S_IRUSR);
    server_config_t* config = cfgparser_parse_server_config(configfd);
    close(configfd);

    int one = 1;

    int server;
    struct sockaddr_in* server_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

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

    pollfd_count = 1;
    pollfds      = (struct pollfd*)malloc(sizeof(struct pollfd));
    clients      = (struct client**)malloc(1);

    pollfds[0].fd = server;
    pollfds[0].events = POLLIN;

    while (1) {
#ifdef WIN32
        WSAPoll(pollfds, pollfd_count, -1);
#else
        poll(pollfds, pollfd_count, -1);
#endif
        if (pollfds[0].revents & POLLIN) {
            struct sockaddr_in* addr     = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
            int*                addr_len = (int*)malloc(4);
            addr_len[0]                  = sizeof(struct sockaddr_in);

#ifdef WIN32
            int fd = accept(server, (struct sockaddr*)addr, addr_len);
#else
            int fd = accept(server, (struct sockaddr*)addr, (socklen_t*)addr_len);
#endif
            client_count++;
            clients = (struct client**)realloc(clients, sizeof(void*) * client_count);
            struct client* c = (struct client*)malloc(sizeof(struct client));
            clients[client_count - 1] = c;
            
            c->nickname = (char*)malloc(5);
            strcpy(c->nickname, "anon");

            c->connection = new ircreborn_connection_t(fd);
            c->state = 0;
            c->queue_gc_ticks = 0;

            ircreborn_phello_t hello;
            hello.ident = "IRCREBORN_REFERENCE_SERVER";
            hello.ident_length = 27;
            hello.protocol_count = protocol_count;
            hello.protocols = protocols;
            hello.master = 0;
            c->connection->send_hello(&hello);

            pollfd_count++;
            pollfds = (struct pollfd*)realloc(pollfds, pollfd_count * sizeof(struct pollfd));
            pollfds[pollfd_count - 1].fd = fd;
            pollfds[pollfd_count - 1].events = POLLIN;
            pollfds[pollfd_count - 1].revents = 0;
        }

        for (int i = 1; i < pollfd_count; i++) {            
            if (pollfds[i].revents & POLLERR) {
                disconnect_socket(pollfds[i].fd, 1, 1, 1, "fatal error");
            } else if (pollfds[i].revents & POLLHUP) {
                disconnect_socket(pollfds[i].fd, 1, 1, 1, "connection lost");
            } else if (pollfds[i].revents & POLLIN) {
                struct client* c = find_client(pollfds[i].fd);

                c->queue_gc_ticks++;

                if (c->queue_gc_ticks > 255) {
                    c->connection->queue_compact();
                    c->queue_gc_ticks = 0;
                }

                c->connection->recv_packet();

                ircreborn_packet_t* packet = c->connection->queue_get(0);

                if (packet == 0) {
                    close(pollfds[i].fd);
                    disconnect_socket(pollfds[i].fd, 1, 1, 1, "malformed packet");
                    goto done;
                }
                
                if (packet->opcode == IRCREBORN_PROTO_V1_OP::HELLO) {
                    ircreborn_phello_t* p = c->connection->queue_get_hello(1);

                    if (p->master) {
                        // set connection to wait for selection
                        c->state = 1;
                    } else {
                        uint32_t sel = 0;

                        for (int i = 0; i < p->protocol_count; i++) {
                            for (int ii = 0; ii < protocol_count; ii++) {
                                if (p->protocols[i] == protocols[ii]) {
                                    if (sel < protocols[ii]) {
                                        sel = protocols[ii];
                                        break;
                                    }
                                }
                            }
                        }

                        if (sel != 0) {
                            ircreborn_pset_proto_t sp;
                            sp.protocol = sel;
                            c->connection->send_set_proto(&sp);
                            c->connection->protocol_version = sel;
                            c->state = 2;

                            set_nickname(c, "anon");
                        } else {
                            disconnect_client(c, 0, 1, 1, "not supported");
                        }
                    }

                    free(p->ident);
                    free(p->protocols);
                    free(p);
                } else if ((packet->opcode == IRCREBORN_PROTO_V1_OP::SET_PROTO) && (c->state == 1)) {
                    ircreborn_pset_proto_t* p = c->connection->queue_get_set_proto(1);
                    c->connection->protocol_version = p->protocol;
                    c->state = 2;
                    
                    free(p);

                    set_nickname(c, "anon");
                } else if ((packet->opcode == IRCREBORN_PROTO_V1_OP::SET_NICKNAME) && (c->state == 2)) {
                    ircreborn_pset_nickname_t* p = c->connection->queue_get_set_nickname(1);

                    set_nickname(c, p->nickname);

                    free(p->nickname);
                    free(p);
                } else if ((packet->opcode == IRCREBORN_PROTO_V1_OP::SEND_MESSAGE) && (c->state == 2)) {
                    ircreborn_psend_message_t* p = c->connection->queue_get_send_message(1);

                    send_all_message(p->message, c->nickname);

                    free(p->message);
                    free(p);
                } else {
                    // drop the broken packet.
                    ircreborn_packet_t* p = c->connection->queue_get(1);
                    
                    free(p->payload);
                    free(p);
                }

                done:;
            }
        }
    }
}