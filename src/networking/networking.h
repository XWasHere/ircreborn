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

#ifndef IRCREBORN_NETWORKING_H
#define IRCREBORN_NETWORKING_H

#include <stdint.h>
#include <networking/packet.h>

void write_int(void* buf, uint32_t i);
uint32_t read_int(void* buf);

#define IRCREBORN_PROTO_V1_OP (uint8_t)__IRCREBORN_PROTO_V1_OP
enum struct __IRCREBORN_PROTO_V1_OP : uint8_t {
    HELLO,
    SET_PROTO,
    SEND_MESSAGE,
    RECV_MESSAGE,
    SET_NICKNAME,
    NICKNAME_UPDATED
};

// packet
typedef struct ircreborn_packet ircreborn_packet_t;
struct ircreborn_packet {
    // an id to help with replies
    uint32_t id;

    // opcode
    uint8_t  opcode;

    // payload
    uint32_t payload_length;
    uint8_t* payload;
};

// hello. sent to begin protocol selection
typedef struct ircreborn_phello ircreborn_phello_t;
struct ircreborn_phello {
    // client identification string
    int       ident_length;
    char*     ident;

    // supported protocol versions
    int       protocol_count;
    uint32_t* protocols;

    // which side selects the protocol. 0 for server 1 for client. (only applicable to client)
    int       master;
};

// selects a protocol
typedef struct ircreborn_pset_proto ircreborn_pset_proto_t;
struct ircreborn_pset_proto {
    uint32_t protocol;
};

// it sends a message
typedef struct ircreborn_psend_message ircreborn_psend_message_t;
struct ircreborn_psend_message {
    // content.
    int   message_length;
    char* message;
};

// message from server
typedef struct ircreborn_precv_message ircreborn_precv_message_t;
struct ircreborn_precv_message {
    // content
    int   message_length;
    char* message;

    // author
    int   author_length;
    char* author;
};

// set nickname
typedef struct ircreborn_pset_nickname ircreborn_pset_nickname_t;
struct ircreborn_pset_nickname {
    // new nickname
    int   nickname_length;
    char* nickname;
};

// nickname was updated
typedef struct ircreborn_pnickname_updated ircreborn_pnickname_updated_t;
struct ircreborn_pnickname_updated {
    // new nickname
    int   nickname_length;
    char* nickname;
};

typedef struct ircreborn_connection ircreborn_connection_t;
struct ircreborn_connection {
    // socket fd
    int fd;

    // protocol used
    int protocol_version;
    
    // next packet id
    int next_id;

    // the queue
    ircreborn_packet_t** queue;
    uint32_t             queue_bottom;
    uint32_t             queue_top;
    uint32_t             queue_size;
    uint32_t             queue_inuse;

    ircreborn_connection(int fd);
    ~ircreborn_connection();

    // send a packet
    int send_packet          (ircreborn_packet_t* packet);
    int send_hello           (ircreborn_phello_t* packet);
    int send_set_proto       (ircreborn_pset_proto_t* packet);
    int send_set_nickname    (ircreborn_pset_nickname_t* packet);
    int send_nickname_updated(ircreborn_pnickname_updated_t* packet);
    int send_recv_message    (ircreborn_precv_message_t* packet);
    int send_send_message    (ircreborn_psend_message_t* packet);
    
    // recieve a packet and add it to the queue  
    int recv_packet();

    // add a packet to the queue
    void queue_add(ircreborn_packet_t* packet);

    // get a packet from the queue
    ircreborn_packet_t*            queue_get                 (int consume);
    ircreborn_phello_t*            queue_get_hello           (int consume);
    ircreborn_pset_proto_t*        queue_get_set_proto       (int consume);
    ircreborn_pset_nickname_t*     queue_get_set_nickname    (int consume);
    ircreborn_pnickname_updated_t* queue_get_nickname_updated(int consume);
    ircreborn_precv_message_t*     queue_get_recv_message    (int consume);
    ircreborn_psend_message_t*     queue_get_send_message    (int consume);
};

#endif