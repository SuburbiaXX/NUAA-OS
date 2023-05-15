#include <utils/root.h>
#include <mm/page.h>
#include "mmu.h"

pte_t *pde_get_pte(pde_t *this, vaddr_t vaddr)
{
    pte_t *first_pte = (pte_t *) pte_vaddr(this);
    return first_pte + pte_index(vaddr); 
}

pgtbl_t *pgtbl_new()
{
    pgtbl_t *this = alloc_page();
    if (!this)
        return NULL;
    memset(this, 0, PAGE_SIZE);
    return this;
}

void pgtbl_compact(pgtbl_t *this)
{
    for (vaddr_t vaddr = USER_START; vaddr < USER_END; vaddr += PDE_COVERED) {
        pde_t *pde = this->dir + (vaddr / PDE_COVERED);

        if (pte_test_bit(pde, PTE_PRESENT)) {
            paddr_t frame = pte_paddr(pde);
            free_frame(frame);
            pte_clear(pde);
        }
    }
}

void pgtbl_delete(pgtbl_t *this)
{
    free_page(this);
}

pte_t *pgtbl_open_pte(pgtbl_t *this, vaddr_t vaddr)
{
    pde_t *pde = this->dir + pde_index(vaddr);
    if (!pte_test_bit(pde, PTE_PRESENT))
        return NULL;
    return pde_get_pte(pde, vaddr);
}

pte_t *pgtbl_create_pte(pgtbl_t *this, vaddr_t vaddr)
{
    pde_t *pde = this->dir + pde_index(vaddr);
    if (!pte_test_bit(pde, PTE_PRESENT)) {
        page_t *page = alloc_page();
        if (!page)
            return NULL;
        memset(page, 0, PAGE_SIZE);

        paddr_t paddr = to_paddr(page);
        pte_make(pde, paddr, PTE_USER | PTE_WRITABLE | PTE_PRESENT);
    }
    return pde_get_pte(pde, vaddr);
}
