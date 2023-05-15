#include <utils/root.h>
#include "ip.h"
#include "nic.h"

void ip_input(packet_t *packet)
{
}

nic_t *ip_route(uint32_t dst_addr)
{
    return NULL;
}

void ip_output(packet_t *packet, uint8_t proto, 
               uint32_t src_addr, uint32_t dst_addr)
{
}
