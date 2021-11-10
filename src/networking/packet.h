#ifndef NETWORKING_PACKET_H
#define NETWORKING_PACKET_H

#define PACKET_HELLO 0x0
typedef struct p_hello hello_t;
struct p_hello {
    // identity (server/client type)
    int   has_ident;
    char* ident;
};

void send_hello(int fd, hello_t* packet);

#endif