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

void write_bool(void* buf, uint8_t value) {
    uint8_t* u8buf = buf;

    u8buf[0] = value ? 0xFF : 0x00;
}

uint8_t read_bool(void* buf) {
    uint8_t* u8buf = buf;

    return u8buf[0];
}

ircreborn_connection::ircreborn_connection(int fd) {
    this->fd = fd;

    this->protocol_version = 1;
    
    this->queue        = malloc(sizeof(void*) * 256);
    memset(queue, 0, sizeof(void*) * 256);
    this->queue_bottom = 0;
    this->queue_size   = 256;
    this->queue_inuse  = 0;
    this->queue_top    = 0;
    
    this->next_id = 0;
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
        memset(data, 0, length + 4);

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
            uint8_t* tmp = malloc(4);
            
            recv(this->fd, tmp, 4, 0);
            length = read_int(tmp);
            
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
        memset(this->queue + sizeof(void*) * (this->queue_size - 256), 0, sizeof(void*) * 256);
    }

    this->queue[this->queue_top] = packet;
    this->queue_top++;
}

ircreborn_packet_t* ircreborn_connection::queue_get(int consume) {
    ircreborn_packet_t* packet = this->queue[this->queue_bottom];
    
    if (consume) {
        this->queue[this->queue_bottom] = 0;
        this->queue_bottom++;
    }

    return packet;
}

int ircreborn_connection::send_hello(ircreborn_phello_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0
            + 4                          // ident length
            + packet->ident_length       // ident
            + 4                          // protocols length
            + packet->protocol_count * 4 // protocols
            + 1;                         // master
        
        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);

        write_int(payload, packet->ident_length);
        memcpy(payload+4, packet->ident, packet->ident_length);
        write_int(payload+4+packet->ident_length, packet->protocol_count);
        memcpy(payload+4+packet->ident_length+4, packet->protocols, packet->protocol_count * 4);
        write_bool(payload+4+packet->ident_length+4+packet->protocol_count*4, packet->master);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::HELLO;
        packet2.payload_length = payload_length;
        packet2.payload = payload;

        int res = this->send_packet(&packet2);

        free(payload);
        
        return res;
    }
    return -1;
}

ircreborn_phello_t* ircreborn_connection::queue_get_hello(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_phello_t* packet = malloc(sizeof(ircreborn_phello_t));
        
        packet->ident_length = read_int(in->payload);
        packet->ident = malloc(packet->ident_length + 1);
        memset(packet->ident, 0, packet->ident_length + 1);
        memcpy(packet->ident, in->payload + 4, packet->ident_length);

        packet->protocol_count = read_int(in->payload + 4 + packet->ident_length);
        packet->protocols = malloc(4 * packet->protocol_count);
        memset(packet->protocols, 0, 4 * packet->protocol_count);
        
        for (int i = 0; i < packet->protocol_count; i++) {
            packet->protocols[i] = read_int(in->payload + 4 + packet->ident_length + 4 + 4 * i);
        }
        
        packet->master = read_bool(in->payload+4+packet->ident_length+4+packet->protocol_count*4);

        free(in->payload);
        free(in);
    
        return packet;
    }
}

int ircreborn_connection::send_set_proto(ircreborn_pset_proto_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0 \
            + 4; // chosen protocol

        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);
        
        write_int(payload, packet->protocol);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::SET_PROTO;
        packet2.payload_length = payload_length;
        packet2.payload = payload;
        int res = this->send_packet(&packet2);

        free(payload);

        return res; 
    }
    return -1;
}

ircreborn_pset_proto_t* ircreborn_connection::queue_get_set_proto(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_pset_proto_t* packet = malloc(sizeof(ircreborn_pset_proto_t));
        packet->protocol = read_int(in->payload);

        free(in->payload);
        free(in);

        return packet;
    }
}

int ircreborn_connection::send_set_nickname(ircreborn_pset_nickname_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0 \
            + 4                        // length
            + packet->nickname_length; // content

        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);

        write_int(payload, packet->nickname_length);
        memcpy(payload+4, packet->nickname, packet->nickname_length);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::SET_NICKNAME;
        packet2.payload_length = payload_length;
        packet2.payload = payload;
        int res = this->send_packet(&packet2);

        free(payload);

        return res;
    }
}

ircreborn_pset_nickname_t* ircreborn_connection::queue_get_set_nickname(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_pset_nickname_t* packet = malloc(sizeof(ircreborn_pset_nickname_t));
        packet->nickname_length = read_int(in->payload);
        packet->nickname        = malloc(packet->nickname_length + 1);
        memset(packet->nickname, 0, packet->nickname_length + 1);
        memcpy(packet->nickname, in->payload + 4, packet->nickname_length);

        free(in->payload);
        free(in);

        return packet;
    }
}

int ircreborn_connection::send_nickname_updated(ircreborn_pnickname_updated_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0 \
            + 4                        // length
            + packet->nickname_length; // content

        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);
        
        write_int(payload, packet->nickname_length);
        memcpy(payload+4, packet->nickname, packet->nickname_length);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::NICKNAME_UPDATED;
        packet2.payload_length = payload_length;
        packet2.payload = payload;
        int res = this->send_packet(&packet2);

        free(payload);

        return res;
    }
}

ircreborn_pnickname_updated_t* ircreborn_connection::queue_get_nickname_updated(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_pnickname_updated_t* packet = malloc(sizeof(ircreborn_pnickname_updated_t));
        packet->nickname_length = read_int(in->payload);
        packet->nickname        = malloc(packet->nickname_length + 1);
        memset(packet->nickname, 0, packet->nickname_length + 1);
        memcpy(packet->nickname, in->payload + 4, packet->nickname_length);

        free(in->payload);
        free(in);

        return packet;
    }
}

int ircreborn_connection::send_recv_message(ircreborn_precv_message_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0
            + 4                      // content length
            + packet->message_length // content
            + 4                      // author length
            + packet->author_length; // author
        
        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);

        write_int(payload, packet->message_length);
        memcpy(payload+4, packet->message, packet->message_length);
        write_int(payload+4+packet->message_length, packet->author_length);
        memcpy(payload+4+packet->message_length+4, packet->author, packet->author_length);
        
        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::RECV_MESSAGE;
        packet2.payload_length = payload_length;
        packet2.payload = payload;

        int res = this->send_packet(&packet2);

        free(payload);
        
        return res;
    }
    return -1;
}

ircreborn_precv_message_t* ircreborn_connection::queue_get_recv_message(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_precv_message_t* packet = malloc(sizeof(ircreborn_precv_message_t));
        
        packet->message_length = read_int(in->payload);
        packet->message = malloc(packet->message_length + 1);
        memset(packet->message, 0, packet->message_length + 1);
        memcpy(packet->message, in->payload + 4, packet->message_length);

        packet->author_length = read_int(in->payload + 4 + packet->message_length);
        packet->author = malloc(packet->author_length+1);
        memset(packet->author, 0, packet->author_length+1);
        memcpy(packet->author, in->payload + 4 + packet->message_length + 4, packet->author_length);
        
        free(in->payload);
        free(in);

        return packet;
    }
}

int ircreborn_connection::send_send_message(ircreborn_psend_message_t* packet) {
    if (this->protocol_version == 1) {
        uint32_t payload_length = 0 \
            + 4                        // length
            + packet->message_length;  // content

        uint8_t* payload = malloc(payload_length);
        memset(payload, 0, payload_length);
        
        write_int(payload, packet->message_length);
        memcpy(payload+4, packet->message, packet->message_length);

        ircreborn_packet_t packet2;
        packet2.opcode = IRCREBORN_PROTO_V1_OP::SEND_MESSAGE;
        packet2.payload_length = payload_length;
        packet2.payload = payload;
        int res = this->send_packet(&packet2);

        free(payload);

        return res;
    }
}

ircreborn_psend_message_t* ircreborn_connection::queue_get_send_message(int consume) {
    if (this->protocol_version == 1) {
        ircreborn_packet_t* in = this->queue_get(consume);

        ircreborn_psend_message_t* packet = malloc(sizeof(ircreborn_psend_message_t));
        packet->message_length = read_int(in->payload);
        packet->message        = malloc(packet->message_length + 1);
        memset(packet->message, 0, packet->message_length + 1);
        memcpy(packet->message, in->payload + 4, packet->message_length);

        free(in->payload);
        free(in);

        return packet;
    }
}

void ircreborn_connection::queue_compact() {
    if (this->queue_bottom > 0) {

    }
}