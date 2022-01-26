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

#include <networking/opcodes.h>
#include <networking/packet.h>
#include <networking/networking.h>
#include <common/util.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#endif

void write_int(void* buf, uint32_t i) {
    uint8_t* u8buf = (uint8_t*)buf;

    u8buf[0] = (i & 0x000000FF);
    u8buf[1] = (i & 0x0000FF00) >> 8;
    u8buf[2] = (i & 0x00FF0000) >> 16;
    u8buf[3] = (i & 0xFF000000) >> 24;
}

uint32_t read_int(void* buf) {
    uint8_t* u8buf = (uint8_t*)buf;

    return u8buf[0] | u8buf[1] << 8 | u8buf[2] << 16 | u8buf[3] << 24;
}

ircreborn_connection::ircreborn_connection(int fd) {
    this->fd = fd;

    this->protocol_version = 1;
    
    this->queue        = malloc(sizeof(void*) * 256);
    this->queue_bottom = 0;
    this->queue_size   = 256;
    this->queue_inuse  = 0;
    this->queue_top    = 0;
}

ircreborn_connection::~ircreborn_connection() {
    free(this->queue);
}

int ircreborn_connection::send_packet(ircreborn_packet_t* packet) {
    if (this->protocol_version == 1) {
        int length = 0
            + 4                       // id
            + 1                       // opcode
            + 4                       // payload length
            + packet->payload_length; // payload

        uint8_t* data = malloc(length + 4);

        write_int(data, length);
        write_int(data + 4, this->next_id);
        data[8] = packet->opcode;
        write_int(data + 9, packet->payload_length);
        
        memcpy(data + 13, packet->payload, packet->payload_length);

        send(this->fd, data, length + 4, 0);

        this->next_id++;

        free(data);

        return this->next_id - 1;
    }

    return -1;
}

int ircreborn_connection::recv_packet() {
    if (this->protocol_version == 1) {
#ifdef WIN32
        unsigned long data = 0;
        ioctlsocket(this->fd, FIONREAD, &data);
#else
        int data = 0;
        ioctl(this->fd, FIONREAD, &data);
#endif
        
        // how much data? who cares?
        // there may be multiple packets so we're better off checking
        // the packet headers instead
        if (data) {
            uint32_t length;

            length = read_int(&length);

            uint8_t* msg = malloc(length);
            recv(this->fd, msg, length, 0);

            ircreborn_packet_t* packet = malloc(sizeof(ircreborn_packet_t));
            packet->id             = read_int(msg);
            packet->opcode         = *(uint8_t*)(msg + 4);
            packet->payload_length = read_int(msg + 5);
            packet->payload        = malloc(packet->payload_length);
            
            memcpy(packet->payload, msg + 9, packet->payload_length);

            free(msg);

            queue_add(packet);

            return 1;
        } else {
            return 0;
        }
    }

    return -1;
}

void ircreborn_connection::queue_add(ircreborn_packet_t* packet) {
    // resize the queue if needed
    if (this->queue_top == this->queue_size) {
        this->queue_size += 256;
        this->queue = realloc(this->queue, sizeof(void*) * this->queue_size);
    }

    this->queue[this->queue_top] = packet;
    this->queue_top++;
}

ircreborn_packet_t* ircreborn_connection::queue_get(int consume) {
    ircreborn_packet_t* packet = this->queue[this->queue_bottom];
    
    if (consume) {
        this->queue_bottom++;
    }

    return packet;
}

int ircreborn_connection::send_hello(ircreborn_phello_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0
            + 4                       // ident length
            + packet->ident_length    // ident
            + 4                       // protocols length
            + packet->protocol_count; // protocols
        
        uint8_t* payload = malloc(payload_length);

        write_int(payload, packet->ident_length);
        memcpy(payload+4, packet->ident, packet->ident_length);
        write_int(payload+4+packet->ident_length, packet->protocol_count);
        memcpy(payload+4+packet->ident_length+4, packet->protocols, packet->protocol_count * 4);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::HELLO;
        packet2.payload_length = payload_length;
        packet2.payload = payload;

        printf("%i\n", payload_length);
        for (int i = 0; i < payload_length; i++) {
            printf("%x ", payload[i]);
        }
        printf("\n");
        int res = this->send_packet(&packet2);

        free(payload);
        
        return res;
    }
    return -1;
}

ircreborn_phello_t* ircreborn_connection::queue_get_hello(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(0);

        ircreborn_phello_t* packet = malloc(sizeof(ircreborn_phello_t));
        
        packet->ident_length = read_int(in->payload);
        packet->ident        = malloc(packet->ident_length + 1);
        memset(packet->ident, 0, packet->ident_length + 1);
        memcpy(packet->ident, in->payload + 4, packet->ident_length);
        printf("%s\n", packet->ident);

        packet->protocol_count = read_int(in->payload + 4 + packet->ident_length);
        packet->protocols = malloc(sizeof(uint32_t) * packet->protocol_count);
        memset(packet->protocols, 0, sizeof(uint32_t) * packet->protocol_count);
        
        for (int i = 0; i < packet->protocol_count; i++) {
            packet->protocols[i] = read_int(in->payload + 4 + packet->ident_length + 4 + sizeof(uint32_t) * i);
        }

        return packet;
    }
}