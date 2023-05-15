#include <utils/root.h>
#include "utils.h"

char *inet_ntoa(uint32_t net)
{
    static char buf_table[4][16];
    static int index = 0;

    char *buf = buf_table[index];
    index = (index + 1) & 3;
    uint8_t *p = (uint8_t *) &net;
    sprintf(buf, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return buf;
}

uint32_t inet_aton(char *c)
{
    uint8_t tmp[4];
    for (int i = 0; i < 4; i++) {
        tmp[i] = 0;
        while ((*c >= '0') && (*c <='9')) {
            tmp[i] *= 10;
            tmp[i] += *c - '0';
            c++;
        }
        c++;
    }

    uint32_t host = (tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];
    return htonl(host);
}
