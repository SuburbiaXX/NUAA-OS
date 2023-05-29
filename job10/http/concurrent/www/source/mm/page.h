#ifndef _MM_PAGE_H
#define _MM_PAGE_H

#include <cpu/mmu.h>

typedef struct page {
    chain_t chain;
    int ref_count;
    paddr_t paddr;
} page_t;

extern page_t page_map[];
extern void page_map_init();
extern void page_map_dump();

extern page_t *paddr_to_page(paddr_t paddr);
extern page_t *vaddr_to_page(vaddr_t vaddr);

extern void page_hold(page_t *this);
extern void page_free(page_t *this);

extern paddr_t alloc_frame();
extern void free_frame(paddr_t paddr);

extern void *alloc_page();
extern void free_page(void *pointer);

#endif
