#ifndef _KERNEL_SYNC_H
#define _KERNEL_SYNC_H

#include <utils/list.h>

extern void schedule();
extern void turn_into_zombie();

typedef list_t cond_t;
extern void cond_init(cond_t *this);
extern void cond_destroy(cond_t *this);
extern void cond_wait(cond_t *this);
extern void cond_signal(cond_t *this);
extern void cond_broadcast(cond_t *this);

#endif
