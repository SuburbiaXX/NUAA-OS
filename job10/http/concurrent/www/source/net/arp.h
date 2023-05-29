#ifndef _NET_ARP_H
#define _NET_ARP_H

#include "packet.h"
#include "nic.h"

#define ARP_OP_REQUEST  1
#define ARP_OP_REPLY    2
#define ARP_HW_NETROM   0
#define ARP_HW_ETH      1
#define ARP_HW_EXPETHER 2  /* experimental ethernet */
#define ARP_HW_AX25     3

typedef struct {
    uint16_t hw_type;
    uint16_t pr_type;
    uint8_t hw_addr_size;
    uint8_t pr_addr_size;
    uint16_t opcode;
    uint8_t data[0];
#if 0
    uint8_t source_hw_addr[6];
    uint8_t source_pr_addr[4];
    uint8_t target_hw_addr[6];
    uint8_t target_pr_addr[4];
#endif
} gcc_packed arp_head_t;

#define ARP_ENTRY_INVALID   11
#define ARP_ENTRY_RESOLVING 22
#define ARP_ENTRY_RESOLVED  33
typedef struct {
    int state;
    uint8_t hw_addr[6];
    uint32_t ip_addr;
} arp_entry_t;

extern void arp_input(packet_t *packet);
extern void arp_output(packet_t *packet);
extern void arp_request(uint32_t ip_addr, nic_t *nic);

#endif
