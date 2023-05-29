#ifndef _NET_ICMP_H
#define _NET_ICMP_H

#include "packet.h"
extern void icmp_input(packet_t *packet);
extern void icmp_output(packet_t *packet);

#endif
