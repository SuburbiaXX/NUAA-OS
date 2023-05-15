#ifndef _NET_UDP_H
#define _NET_UDP_H

#include "socket.h"

typedef struct tcp_socket {
    socket_member_list;
    uint32_t local_addr;
    uint16_t local_port;
    uint32_t remote_addr;
    uint16_t remote_port;
} udp_socket_t;

#endif
