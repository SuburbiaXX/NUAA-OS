#include <utils/root.h>
#include <mm/memory.h>
#include "arp.h"
#include "nic.h"
#include "eth.h"

int arp_entry_count;
arp_entry_t *arp_entry_table;

void arp_entry_init(arp_entry_t *this)
{
    this->state = ARP_ENTRY_INVALID;
    memset(this->hw_addr, 0, 6);
    this->ip_addr = 0;
}

void arp_init()
{
    arp_entry_count = 8;
    arp_entry_table = alloc_memory(arp_entry_count * sizeof(arp_entry_t));
    for (int i = 0; i < arp_entry_count; i++) {
        arp_entry_t *arp_entry = arp_entry_table + i;
        arp_entry_init(arp_entry);
    }
}

arp_entry_t *arp_lookup(uint32_t ip_addr)
{
    for (int i = 0; i < arp_entry_count; i++) {
        arp_entry_t *arp_entry = arp_entry_table + i;
    }
    return NULL; 
}

void arp_input(packet_t *packet)
{
}

void arp_output(packet_t *packet)
{
}

/* broadcast an arp request packet */ 
void arp_request(uint32_t ip_addr, nic_t *nic)
{
    int hw_addr_size = 6;
    int pr_addr_size = 4;
    int body_size = sizeof(arp_head_t) + (hw_addr_size + pr_addr_size) * 2;
    packet_t *packet = packet_new(LINK_HEAD, body_size);

    arp_head_t *ah = packet->data;
    ah->opcode = htons(ARP_OP_REQUEST);
    ah->hw_type = htons(ARP_HW_ETH);
    ah->pr_type = htons(ETH_PROTO_IP);
    ah->hw_addr_size = hw_addr_size;
    ah->pr_addr_size = pr_addr_size; 

    uint8_t *data = ah->data;
    uint8_t *source_hw_addr = data;
    memcpy(source_hw_addr, nic->hw_addr, hw_addr_size);
    data += hw_addr_size;

    uint8_t *source_pr_addr = data;
    encode_uint32(source_pr_addr, nic->ip_addr);
    data += pr_addr_size;

    uint8_t *target_hw_addr = data; 
    memset(target_hw_addr, 0, hw_addr_size);
    data += hw_addr_size;

    uint8_t *target_pr_addr = data;
    encode_uint32(target_pr_addr, ip_addr);
    data += pr_addr_size;

    nic_broadcast(nic, packet, ETH_PROTO_ARP);
}

void arp_head_dump(arp_head_t *this)
{
    printf("arp_head\n");
    printf("hw_type = %d\n", ntohs(this->hw_type));
    printf("pr_type = %d\n", ntohs(this->pr_type));
    printf("hw_addr_size = %d\n", this->hw_addr_size);
    printf("pr_addr_size = %d\n", this->pr_addr_size);
    printf("opcode = %d\n", ntohs(this->opcode));

    int hw_addr_size = 6;
    int pr_addr_size = 4;
    void *data = this->data;

    printf("source_hw_addr = %s\n", eth_addr_to_string(data));
    data += hw_addr_size;

    uint32_t source_pr_addr = *(uint32_t *)data;
    printf("source_pr_addr = %s\n", inet_ntoa(source_pr_addr));
    data += pr_addr_size;

    printf("target_hw_addr = %s\n", eth_addr_to_string(data));
    data += hw_addr_size;

    uint32_t target_pr_addr = *(uint32_t *)data;
    printf("target_pr_addr = %s\n", inet_ntoa(target_pr_addr));
    data += pr_addr_size;
}
