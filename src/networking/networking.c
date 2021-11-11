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
    char* obuf = malloc(255);
    int   olen = 0;

    olen = 8 + 1 + 4 + strlen(packet->ident);
    
    write_int    (obuf,     PACKET_HELLO);
    write_int    (obuf + 4, olen - 8);    
    write_bool   (obuf + 8, packet->has_ident);
    write_string (obuf + 9, packet->ident, strlen(packet->ident));

    send(sock, obuf, olen, 0);
}