#ifndef _NET_ETH_H
#define _NET_ETH_H

#include "utils.h"
/* +--------+---------+---------+-------+---------+------+
 * | pad(2) |target(6)|source(6)|type(2)|mtu(1500)|crc(4)|
 * +--------+---------+---------+-------+---------+------+ */
#define ETH_PAD         2    /* make the ip header at 4 bytes boundary */
#define ETH_ADDR_SIZE   6    /* Octets in one ethernet addr */
#define ETH_HEAD_SIZE   14   /* Total octets in header.  */
#define ETH_MTU         1500 /* Max. octets in payload */
#define ETH_MIN_FRAME   60
#define ETH_MAX_FRAME   1514

/* These are the defined Ethernet Protocol ID's. */
#define ETH_PROTO_LOOP  0x0060     /* Ethernet Loopback packet     */
#define ETH_PROTO_IP    0x0800     /* Internet Protocol packet     */
#define ETH_PROTO_ARP   0x0806     /* Address Resolution packet    */
#define ETH_PROTO_RARP  0x8035     /* Reverse Addr Res packet      */

/* This is an Ethernet frame header. */
typedef struct {
    uint8_t target_addr[ETH_ADDR_SIZE];  /* destination eth addr */
    uint8_t source_addr[ETH_ADDR_SIZE];  /* source ether addr    */
    uint16_t proto;                      /* packet type ID field */
} eth_head_t;

char *eth_addr_to_string(uint8_t *p);
void eth_head_dump(eth_head_t *this);

#endif
