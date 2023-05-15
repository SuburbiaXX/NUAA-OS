#include <utils/root.h>
#include "nic.h"

#define MAX_NIC_COUNT 4
int nic_count = 0;
nic_t *nic_table[MAX_NIC_COUNT];

void install_nic(nic_t *nic)
{
    nic_table[nic_count] = nic;
    nic_count++;
}

void nic_init(nic_t *this, char *name, nic_output_t output)
{
    strcpy(this->name, name);
    this->ip_addr = 0;
    memset(this->hw_addr, 0, ETH_ADDR_SIZE);
    memset(this->broadcast_addr, 0xFF, ETH_ADDR_SIZE);
    this->output = output;
}

void nic_output(nic_t *this, packet_t *packet, uint8_t *target_addr, int proto)
{
    packet_push(packet, sizeof(eth_head_t));
    eth_head_t *eh = packet->data;
    memcpy(eh->source_addr, this->hw_addr, 6);
    memcpy(eh->target_addr, target_addr, 6);
    eh->proto = htons(proto); 
    this->output(this, packet);
}

void nic_broadcast(nic_t *this, packet_t *packet, int proto)
{
    uint8_t *target_addr = this->broadcast_addr;
    nic_output(this, packet, target_addr, proto);
}
