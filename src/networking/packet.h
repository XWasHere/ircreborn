#ifndef NETWORKING_PACKET_H
#define NETWORKING_PACKET_H

typedef struct p_hello hello_t;
struct p_hello {
    // identity (server/client type)
    int   has_ident;
    char* ident;
};

typedef struct p_message message_t;
struct p_message {
    char* message;
};

void send_hello(int fd, hello_t* packet);
void send_message(int fd, message_t* packet);
#endif