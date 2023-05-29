#ifndef _CPU_MMU_H
#define _CPU_MMU_H

#include "layout.h"

#define PAGE_SIZE 4096
#define PAGE_BITS 12
#define PAGE_0011 (PAGE_SIZE - 1)
#define PAGE_1100 (~PAGE_0011)

#define page_up(address) (((ulong)(address) + PAGE_SIZE - 1) & PAGE_1100)
#define page_down(address) ((ulong)(address) & PAGE_1100)
#define page_number(address) ((ulong)(address) >> PAGE_BITS)
#define page_offset(address) ((ulong)(address) & PAGE_0011)

#define PTE_PRESENT            0x001   
#define PTE_WRITABLE           0x002  
#define PTE_USER               0x004 
#define PTE_WRITE_THROUGH      0x008
#define PTE_CACHE_DISABLE      0x010
#define PTE_ACCESSED           0x020   
#define PTE_DIRTY              0x040  
#define PTE_RESERVE0           0x080           // PS   page size (0=4k,1=4M)   
#define PTE_RESERVE1           0x100           // G    global                  
#define PTE_AVAIL0             0x200           // AVL  available for system  
#define PTE_AVAIL1             0x400           //      programmers use      
#define PTE_AVAIL2             0x800           //                          
#define PTE_FRAME              PAGE_1100       // FRAME page frame address     

/* PDE_COVERED determines what a page directory entry can map */
#define PDE_COVERED            (1UL << 22)

/* page table entry */
typedef uint32_t pte_t;

static inline int pte_test_bit(pte_t *this, int mask) 
{
    return *this & mask;
}

static inline void pte_set_bit(pte_t *this, int mask) 
{
    *this |= mask; 
}

static inline void pte_clear_bit(pte_t *this, int mask) 
{
    *this &= ~mask; 
}

static inline int pte_null(pte_t *this) 
{ 
    return *this == 0; 
}

static inline void pte_clear(pte_t *this) 
{ 
    *this = 0; 
}

static inline uint32_t pte_paddr(pte_t *this) 
{ 
    return *this & PTE_FRAME; 
}

static inline uint32_t pte_vaddr(pte_t *this) 
{ 
    return to_vaddr(*this & PTE_FRAME);
}

static inline void pte_make(pte_t *this, uint32_t paddr, uint32_t attr)
{
    *this = paddr + attr;
}

typedef uint32_t pde_t;

static inline int pde_index(vaddr_t vaddr)
{
    return (vaddr >> 22);
}

static inline int pte_index(vaddr_t vaddr)
{
    return (vaddr >> 12) & 1023;
}

typedef struct pgtbl {
    pde_t dir[1024];
} pgtbl_t;

extern pgtbl_t *pgtbl_new();
extern void pgtbl_compact(pgtbl_t *this);
extern void pgtbl_delete(pgtbl_t *this);
extern pte_t *pgtbl_open_pte(pgtbl_t *this, vaddr_t vaddr);
extern pte_t *pgtbl_create_pte(pgtbl_t *this, vaddr_t vaddr);

#endif
