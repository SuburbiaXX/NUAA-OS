#ifndef _CPU_CPU_H
#define _CPU_CPU_H

#define EFLAG_CF                (1)
#define EFLAG_PF                (1 << 3)
#define EFLAG_IF                (1 << 9)
#define EFLAG_IOPL

#define CR0_PE                  (1)
#define CR0_MP                  (1 << 1)
#define CR0_EM                  (1 << 2)
#define CR0_TS                  (1 << 3)
#define CR0_WP                  (1 << 16)
#define CR0_PG                  (1 << 31)

extern void cli();
extern void sti();

extern void tlb_flush_all();
extern void tlb_flush_page(vaddr_t vaddr);

struct pgtbl;
extern void load_pgtbl(struct pgtbl *pgtbl);

typedef struct {
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t eip;
} context_t;

extern void switch_context(context_t *source, context_t *target);
extern void context_init(context_t *this, void *kernel_stack);
#endif
