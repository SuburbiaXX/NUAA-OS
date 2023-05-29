#ifndef	_NET_INTERFACE_CARD_H
#define _NET_INTERFACE_CARD_H

#include <cpu/irq.h>
#include <cpu/cpu.h>
#include "packet.h"
#include "eth.h"

struct nic;
typedef int (*nic_output_t)(struct nic *this, packet_t *packet);

#define nic_member_list                     \
    char name[16];                          \
    uint8_t hw_addr[ETH_ADDR_SIZE];         \
    uint8_t broadcast_addr[ETH_ADDR_SIZE];  \
    uint32_t ip_addr;                       \
    nic_output_t output

typedef struct nic {
    nic_member_list;
} nic_t;

extern void install_nic(nic_t *nic);
extern void nic_init(nic_t *this, char *name, nic_output_t output);
extern void nic_broadcast(nic_t *this, packet_t *packet, int proto);

#endif
