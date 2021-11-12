#include <networking/opcodes.h>
#include <networking/packet.h>
#include <networking/types.h>
#include <common/util.h>

#include <unistd.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

void send_hello(int sock, hello_t* packet) {
    int   olen = 8 + 1 + 4 + strlen(packet->ident);
    char* obuf = malloc(olen);

    write_int    (obuf,     OPCODE_HELLO);
    write_int    (obuf + 4, olen - 8);    
    write_bool   (obuf + 8, packet->has_ident);
    write_string (obuf + 9, packet->ident, strlen(packet->ident));

    send(sock, obuf, olen, 0);
}

void send_message(int sock, message_t* packet) {
    int   olen = 8 + 4 + strlen(packet->message);
    char* obuf = malloc(olen);

    write_int    (obuf,     OPCODE_MESSAGE);
    write_int    (obuf + 4, olen - 8);
    write_string (obuf + 8, packet->message, strlen(packet->message));

    send(sock, obuf, olen, 0);
}