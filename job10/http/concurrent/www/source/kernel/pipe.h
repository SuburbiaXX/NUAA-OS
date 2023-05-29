#ifndef _KERNEL_PIPE_H
#define _KERNEL_PIPE_H

#include <utils/fifo.h>
#include "sync.h"

typedef struct pipe {
    fifo_t fifo;
    cond_t cond;
    int reader_count;
    int writer_count;
} pipe_t;

#endif
