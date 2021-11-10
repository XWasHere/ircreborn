#include <networking/opcodes.h>
#include <networking/packet.h>
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
    
    ((int*)obuf)[0] = OPCODE_HELLO;
    ((int*)obuf)[1] = olen - 8;
    
    obuf[8] = packet->has_ident ? 1 : 0;
    
    ((int*)obuf + 9)[0] = strlen(packet->ident);
    
    memcpy(obuf + 13, packet->ident, strlen(packet->ident));
    send(sock, obuf, olen, 0);
}