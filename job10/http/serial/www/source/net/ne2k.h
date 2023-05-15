#ifndef _NE2000_H
#define _NE2000_H

#include "nic.h"
#include "8390.h"

typedef struct {
    nic_member_list;
    int io_base;
    int irq_no;
    int tx_start_page;
    int tx_end_page;
    int rx_start_page;
    int rx_end_page;
} ne2k_t;

#endif
