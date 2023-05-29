#ifndef	_NET_PACKET_H
#define _NET_PACKET_H

#include "utils.h"
/**         
 * +---------------+----------------+
 * |     head      |      data      |
 * +---------------+----------------+
 * ^               ^                ^
 * |               |                |
 * room            data             bound
 */
typedef struct {
    chain_t chain;
    void *room;
    void *data;
    void *bound;
    int data_size;
} packet_t;

extern packet_t *packet_new(int head_size, int data_size);
extern void packet_delete(packet_t *this);
extern void packet_push(packet_t *this, int head_size);
extern void packet_pop(packet_t *this, int head_size);

#define LINK_PAD 2    /* make the ip header at 4 bytes boundary */
#define LINK_HEAD (LINK_PAD + sizeof(eth_head_t))
#define LINK_IP_HEAD (LINK_HEAD + sizeof(ip_head_t))
#define LINK_IP_TCP_HEAD (LINK_IP_HEAD + sizeof(tcp_head_t))
#define LINK_IP_UDP_HEAD (LINK_IP_HEAD + sizeof(udp_head_t))
#define LINK_IP_ICMP_HEAD (LINK_IP_HEAD + sizeof(icmp_head_t))

#endif
