#ifndef _MM_USTACK_H
#define _MM_USTACK_H

typedef struct {
    void *page;
    void *sp;
    vaddr_t base;

    int argc;
    long *argp;
    int envc;
    long *envp;
} ustack_t;

extern void ustack_init(ustack_t *this);
extern int ustack_build(ustack_t *this, char *argv[], char *envv[]);
extern vaddr_t ustack_map(ustack_t *this);

#endif
