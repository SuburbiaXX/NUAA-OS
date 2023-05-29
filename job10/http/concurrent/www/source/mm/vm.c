#include <utils/root.h>
#include <kernel/pcb.h>
#include <kernel/errno.h>
#include <cpu/mmu.h>
#include <cpu/cpu.h>
#include "page.h"
#include "vm.h"

pgtbl_t kernel_pgtbl gcc_aligned(PAGE_SIZE);

void kernel_pgtbl_init()
{
    vaddr_t vaddr;
    paddr_t paddr;

    puts("init kernel pgtbl");
    for (vaddr = KERNEL_START; vaddr < KERNEL_END; vaddr += PAGE_SIZE) {
        pte_t *pte = pgtbl_create_pte(&kernel_pgtbl, vaddr);
        paddr = to_paddr(vaddr);
        pte_make(pte, paddr, PTE_WRITABLE | PTE_PRESENT);
    }
    load_pgtbl(&kernel_pgtbl);
}

void kernel_pgtbl_share(pgtbl_t *pgtbl)
{
    vaddr_t vaddr;

    for (vaddr = KERNEL_START; vaddr < KERNEL_END; vaddr += PDE_COVERED) {
        pde_t *source_pde = kernel_pgtbl.dir + pde_index(vaddr);
        pde_t *target_pde = pgtbl->dir + pde_index(vaddr);
        *target_pde = *source_pde;
    }
}

void copy_frame(paddr_t this_frame, paddr_t that_frame)
{
    void *this_page = (void *)to_vaddr(this_frame);
    void *that_page = (void *)to_vaddr(that_frame);
    memcpy(that_page, this_page, PAGE_SIZE);
}

void map_range(pgtbl_t *pgtbl, vaddr_t start, vaddr_t end, int protect)
{
    start = page_down(start); 
    end = page_up(end);
    for (vaddr_t vaddr = start; vaddr < end; vaddr += PAGE_SIZE) {
        pte_t *pte = pgtbl_create_pte(pgtbl, vaddr);
        assert(pte_null(pte));
        paddr_t paddr = alloc_frame();
        pte_make(pte, paddr, protect);
    }
}

void map_segment(pgtbl_t *pgtbl, segment_t *segment, int protect)
{
    vaddr_t start = segment->start;
    vaddr_t end = segment->end;
    map_range(pgtbl, start, end, protect);
}

void copy_segment(pgtbl_t *this_pgtbl, segment_t *this_segment, 
                  pgtbl_t *that_pgtbl, segment_t *that_segment)
{
    pte_t *this_pte, *that_pte;
    that_segment->start = this_segment->start;
    that_segment->end = this_segment->end;

    vaddr_t start = page_down(this_segment->start);
    vaddr_t end = page_up(this_segment->end);
    for (vaddr_t vaddr = start; vaddr < end; vaddr += PAGE_SIZE) {
        paddr_t this_frame;
        paddr_t that_frame;

        this_pte = pgtbl_open_pte(this_pgtbl, vaddr);
        that_pte = pgtbl_create_pte(that_pgtbl, vaddr);
        assert(that_pte != NULL);

        this_frame = pte_paddr(this_pte);
        that_frame = alloc_frame();
        copy_frame(this_frame, that_frame);
        pte_make(that_pte, that_frame, PTE_USER|PTE_WRITABLE|PTE_PRESENT);
    }
}

void copy_vm(pcb_t *that_pcb)
{
    pgtbl_t *this_pgtbl = this_pcb->pgtbl;
    pgtbl_t *that_pgtbl = pgtbl_new();
    kernel_pgtbl_share(that_pgtbl);

    that_pcb->pgtbl = that_pgtbl;
    that_pcb->kernel_stack = alloc_page();

    for (int i = 0; i < SEGMENT_NUMBER; i++) {
        segment_t *this_segment = this_pcb->segment_array + i;
        segment_t *that_segment = that_pcb->segment_array + i;
        copy_segment(this_pgtbl, this_segment, that_pgtbl, that_segment);
    }
}

void flush_segment(pgtbl_t *pgtbl, segment_t *segment)
{
    vaddr_t start = page_down(segment->start);
    vaddr_t end = page_up(segment->end);

    for (vaddr_t vaddr = start; vaddr < end; vaddr += PAGE_SIZE) {
        pte_t *pte = pgtbl_open_pte(pgtbl, vaddr);
        paddr_t frame = pte_paddr(pte);
        free_frame(frame);
        pte_clear(pte);
    }
    segment->start = 0;
    segment->end = 0;
}

void flush_vm()
{
    int i;

    for (i = 0; i < SEGMENT_NUMBER; i++) {
        segment_t *segment = this_pcb->segment_array + i;
        flush_segment(this_pcb->pgtbl, segment);
    }
    pgtbl_compact(this_pcb->pgtbl);
    tlb_flush_all();
}

int verify_page(vaddr_t vaddr, int mask)
{
    pte_t *pte = pgtbl_open_pte(this_pcb->pgtbl, vaddr);
    if (pte == NULL)
        return EFAULT;
    if ((*pte & mask) != mask)
        return EFAULT;
    return 0;
}

int verify_area(void *buffer, size_t size, int mask)
{
    vaddr_t start = page_down(buffer);
    vaddr_t end = page_up(buffer + size);

    for (vaddr_t vaddr = start; vaddr < end; vaddr += PAGE_SIZE) {
        int error = verify_page(vaddr, mask);
        if (error < 0)
            return error;
    }
    return 0;
}

int verify_read(void *buffer, size_t size)
{
    return verify_area(buffer, size, PTE_PRESENT);
}

int verify_write(void *buffer, size_t size)
{
    return verify_area(buffer, size, PTE_WRITABLE | PTE_PRESENT);
}

int verify_string(char *string)
{
    return verify_page((vaddr_t)string, PTE_PRESENT);
}

int verify_path(char *path)
{
    int error = verify_string(path);
    if (error < 0)
        return error;

    if (strlen(path) >= PATH_MAX)
        return ENAMETOOLONG;
    return 0;
}

// end with NULL
int verify_vector(char *vector[])
{
    int count = 0;
    char **p;

    for (p = vector; *p; p++)
        count++;
    return count;
}

ulong sys_brk(vaddr_t new_end)
{
    segment_t *data_segment = &this_pcb->data_segment;
    segment_t *stack_segment = &this_pcb->stack_segment;

    vaddr_t old_end = data_segment->end;
    if (new_end == 0)
        return old_end;
    if (new_end < old_end || new_end >= stack_segment->start)
        return EINVAL;

    data_segment->end = new_end;
    old_end = page_up(old_end);
    int protect = PTE_USER|PTE_WRITABLE|PTE_PRESENT;
    map_range(this_pcb->pgtbl, old_end, new_end, protect);
    return 0;
}
