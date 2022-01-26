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
    int has_nickname;
    char* nickname;
};

int             pollfd_count = 0;
struct pollfd*  pollfds;
int             client_count = 0;
struct client** clients;

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
    message_t* message = malloc(sizeof(message_t));

    message->message = msg;
    message->name    = name;

//    send_message(client->connection->fd, message);

    free(message);
}

void send_all_message(char* msg, char* name) {
    for (int i = 0; i < client_count; i++) {
        send_client_message(clients[i], msg, name);
    }
}

void disconnect_client(struct client* client, int send_message, int automatic, int has_reason, char* reason) {
    int cid = find_client_id_from_client(client);
    
    for (int i = cid; i < client_count - 1; i++) {
        clients[i] = clients[i + 1];
    }

    if (send_message) {
        char* buf;
        asprintf(
            &buf,
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
                        
            pollfd_count++;
            pollfds = (struct pollfd*)realloc(pollfds, pollfd_count * sizeof(struct pollfd));
            pollfds[pollfd_count - 1].fd = fd;
            pollfds[pollfd_count - 1].events = POLLIN;
        }

        for (int i = 1; i < pollfd_count; i++) {
            if (pollfds[i].revents & POLLERR) {
                logger->log(CHANNEL_FATL, "error\n");
                disconnect_socket(pollfds[i].fd, 1, 1, 1, "fatal error");
            } else if (pollfds[i].revents & POLLHUP) {
                disconnect_socket(pollfds[i].fd, 1, 1, 1, "connection lost");
            } else if (pollfds[i].revents & POLLIN) {
                struct client* c = find_client(pollfds[i].fd);

                c->connection->recv_packet();

                ircreborn_packet_t* packet = c->connection->queue_get(0);

                if (packet == 0) {
                    close(pollfds[i].fd);
                    disconnect_socket(pollfds[i].fd, 1, 1, 1, "malformed packet");
                    goto done;
                }
                
                if (packet->opcode == IRCREBORN_PROTO_V1_OP::HELLO) {
                    logger->log(CHANNEL_DBUG, "GOT HELLO\n");

                    ircreborn_phello_t* p = c->connection->queue_get_hello(1);

                    printf("IDENT \"%s\" PROTO COUNT %i\n", p->ident, p->protocol_count);

                    free(packet);
                    free(p);
                } else {
                    free(c->connection->queue_get(1));
                }
                done:;
/*
                unused char c;
                char* msgbuf = 0;
                unused int   bufpos = 0;

                void* header = malloc(8);
                int   buflen = 0;
                int   op     = 0;

                if (recv(pollfds[i].fd, (char*)header, 8, 0) == 0) {
                    logger->log(CHANNEL_DBUG, "got 0 bytes of data. assuming broken connection. kicking\n");
                    close(pollfds[i].fd);
                    disconnect_socket(pollfds[i].fd, 1, 1, 1, "read error");
                } else {
                    op     = read_int(header);
                    buflen = read_int(header + 4);
                    
                    msgbuf = (char*)malloc(buflen + 1);
                    memset(msgbuf, 0, buflen + 1);

                    unused int res = recv(pollfds[i].fd, msgbuf, buflen, 0);

                    if (op == OPCODE_HELLO) {
                        unused int has_ident    = read_bool(msgbuf);
                        unused nstring_t* ident = read_string(msgbuf + 1);
                        client_count++;
                        clients = (struct client**)realloc(clients, sizeof(void*) * client_count);
                        struct client* c = (struct client*)malloc(sizeof(struct client));
                        clients[client_count - 1] = c;
                        c->nickname = (char*)malloc(5);
                        
                        strcpy(c->nickname, "anon");
                        c->fd = pollfds[i].fd;
                        set_nickname_t* snpacket = (set_nickname_t*)malloc(sizeof(set_nickname_t));
                        snpacket->nickname = "anon";
                        send_set_nickname(c->connection->fd, snpacket);
                        free(snpacket);
                    } else if (op == OPCODE_MESSAGE) {
                        struct client* c = find_client(pollfds[i].fd);
                        nstring_t* imsg = read_string(msgbuf);
                        send_all_message(imsg->str, c->nickname);
                    } else if (op == OPCODE_SET_NICKNAME) {
                        struct client* c = find_client(pollfds[i].fd);
                        nstring_t* nick = read_string(msgbuf);
                        set_nickname_t* packet = (set_nickname_t*)malloc(sizeof(set_nickname_t));

                        logger->log(CHANNEL_DBUG, "client set nickname %s -> %s\n", c->nickname, nick->str);

                        c->nickname = nick->str;
                        packet->nickname = c->nickname;

                        send_set_nickname(c->connection->fd, packet);

                        free(packet);
                        free(nick);
                    }

                    free(msgbuf);
                    free(header);
                    fflush(0);
                }*/
            }
        }
    }
}