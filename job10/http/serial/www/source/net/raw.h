#ifndef _NET_RAW_H
#define _NET_RAW_H

#include "socket.h"

typedef struct {
    socket_member_list;
    uint32_t local_addr;
    uint16_t local_port;
    uint32_t foreign_addr;
    uint16_t foreign_port;
} raw_socket_t;

#endif
