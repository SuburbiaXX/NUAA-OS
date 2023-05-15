#ifndef _NET_TCP_H
#define _NET_TCP_H

#include "socket.h"

typedef struct tcp_socket {
    socket_member_list;
    uint32_t local_addr;
    uint16_t local_port;
    uint32_t remote_addr;
    uint16_t remote_port;
} tcp_socket_t;

#endif
