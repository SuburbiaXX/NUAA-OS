#ifndef _CORO_H
#define _CORO_H

#include "cpu.h"

#define CORO_STACK_SIZE 40960

enum {
    CORO_RUNNING,
    CORO_EXIT
};

typedef struct coro {
    cpu_t cpu;
    void *stack;
    struct coro *back;
} coro_t;

extern void coro_boot(coro_t *that_coro);
extern coro_t *coro_new(void (*entry)());
extern void coro_delete(coro_t *coro);
extern unsigned long coro_resume(coro_t *that_coro);
extern void coro_yield(unsigned long value);

#endif
