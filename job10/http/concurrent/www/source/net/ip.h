#ifndef _NET_IP_H
#define _NET_IP_H

#include "packet.h"

#define IP_FRAG_OFFSET    0x1fff 
#define IP_FRAG_MORE      0x2000 /* more frag */
#define IP_FRAG_DONT      0x4000 /* don't frag */
#define IP_FRAG_RESERVE   0x8000

#define IP_VER4 4
typedef struct {
    unsigned head_size:4; 
    unsigned version:4;
    uint8_t tos;                /* type of service */
    uint16_t total_size;        /* total size, include header */
    uint16_t id;
    uint16_t frag;
    uint8_t ttl;                /* time to live */
    uint8_t proto;              /* protocol */
    uint16_t check_sum;
    uint32_t source_addr;
    uint32_t target_addr;
    char data[0];
} gcc_packed ip_head_t;

extern void ip_input(packet_t *packet);

#endif
