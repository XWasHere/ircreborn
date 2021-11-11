#include <stdio.h>

#include <common/args.h>
#include <common/util.h>
#include <networking/networking.h>
#include <networking/types.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#endif

void server_main() {
#ifdef WIN32
    WSADATA* wsadata = malloc(sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2,2), wsadata)) {
        printf(FMT_FATL("failed to start winsock, aborting\n"));
        printf(FMT_FATL("%s"), format_error(WSAGetLastError()));
        exit(1);
    }
#endif
    int one = 1;

    int server;
    struct sockaddr_in* server_addr = malloc(sizeof(struct sockaddr_in));

    server = socket(AF_INET, SOCK_STREAM, 0);

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(args_listen_port);

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    bind(server, server_addr, sizeof(struct sockaddr_in));

    listen(server, 3);

    int            pollfd_count = 1;
    struct pollfd* pollfds      = malloc(sizeof(struct pollfd));
    
    pollfds[0].fd = server;
    pollfds[0].events = POLLIN;

    while (1) {
#ifdef WIN32
        WSAPoll(pollfds, pollfd_count, -1);
#else
        // TODO: this should have code
#endif
        if (pollfds[0].revents & POLLIN) {
            struct sockaddr_in* addr     = malloc(sizeof(struct sockaddr_in));
            int*                addr_len = malloc(4);
            addr_len[0]                  = sizeof(struct sockaddr_in);

            int fd = accept(server, addr, addr_len);
            
            printf(FMT_INFO("accepted\n"));

            pollfd_count++;
            pollfds = realloc(pollfds, pollfd_count * sizeof(struct pollfd));
            pollfds[pollfd_count - 1].fd = fd;
            pollfds[pollfd_count - 1].events = POLLIN;
        }

        for (int i = 1; i < pollfd_count; i++) {
            if (pollfds[i].revents & POLLIN) {
                char c;
                char* msgbuf = 0;
                int   bufpos = 0;

                void* header = malloc(8);
                int   buflen = 0;
                int   op     = 0;

                recv(pollfds[i].fd, header, 8, 0);

                op     = read_int(header);
                buflen = read_int(header + 4);
                
                msgbuf = malloc(buflen + 1);
                memset(msgbuf, 0, buflen + 1);

                int res = recv(pollfds[i].fd, msgbuf, buflen, 0);

                if (op == OPCODE_HELLO) {
                    int has_ident    = read_bool(msgbuf);
                    nstring_t* ident = read_string(msgbuf + 1);
                    if (has_ident) {
                        printf(FMT_INFO("got connection from client of type \"%s\"\n"), ident->str);
                    } else {
                        printf(FMT_INFO("got connection from client of unknown type\n"));
                    }
                }

                free(msgbuf);
                free(header);
                fflush(0);
            } else if (pollfds[i].revents & POLLERR) {
                printf(FMT_INFO("error\n"));
            } else if (pollfds[i].revents & POLLHUP) {
                printf(FMT_INFO("bye\n"));
                for (int ii = i; ii < pollfd_count - 1; ii++) {
                    pollfds[ii] = pollfds[ii + 1];
                }
                pollfd_count--;
            }
        }
    }
}