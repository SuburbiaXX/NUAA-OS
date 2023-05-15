#include <utils/root.h>
#include "eth.h"

char *eth_addr_to_string(uint8_t *p)
{
    static char buf_table[4][32];
    static int index;

    char *buf = buf_table[index];
    index = (index + 1) & 3;

    sprintf(buf, "%x:%x:%x:%x:%x:%x", p[0], p[1], p[2], p[3], p[4], p[5]);
    return buf;
}

void eth_head_dump(eth_head_t *this)
{
    uint8_t *p;

    printf("target: %s\n", eth_addr_to_string(this->target_addr));
    printf("source: %s\n", eth_addr_to_string(this->source_addr));
    uint16_t proto = ntohs(this->proto);
    printf("proto: %x\n", proto);
}


