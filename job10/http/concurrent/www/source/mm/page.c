#include <utils/root.h>
#include "page.h"

page_t page_map[PHYSICAL_PAGES];
list_t free_page_list = {&free_page_list, &free_page_list};
int free_page_count;

void page_init(page_t *this, paddr_t paddr)
{
    chain_init(&this->chain);
    this->ref_count = 1;
    this->paddr = paddr;
}

static void free_frame_range(paddr_t start, paddr_t end)
{
    paddr_t paddr;

    for (paddr = start; paddr < end; paddr += PAGE_SIZE)
        free_frame(paddr);
}

void page_map_init()
{
    int i;
    vaddr_t bss_end; 

    puts("init page map");
    for (i = 0; i < PHYSICAL_PAGES; i++) {
        page_t *page = &page_map[i];
        paddr_t paddr = i*PAGE_SIZE;
        page_init(page, paddr);
    }

    bss_end = page_up(&__bss_end);
    free_frame_range(bss_end, 640*K);
    free_frame_range(1*M, PHYSICAL_MEMORY);
}

void page_map_dump()
{
    printf("free page count = %d\n", free_page_count);
}

page_t *paddr_to_page(paddr_t paddr)
{
    return page_map + page_number(paddr);
}

page_t *vaddr_to_page(vaddr_t vaddr)
{
    paddr_t paddr = to_paddr(vaddr);
    return page_map + page_number(paddr);
}

void page_hold(page_t *this)
{
    this->ref_count++;
}

void page_free(page_t *this)
{
    if (--(this->ref_count) > 0)
        return;

    list_push_back(&free_page_list, this);
    free_page_count++;
}

paddr_t alloc_frame()
{
    page_t *page;

    page = list_pop_front(&free_page_list);
    if (!page)
        return 0;
    assert(page->ref_count == 0);
    free_page_count--;
    page->ref_count = 1;
    return page->paddr;
}

void free_frame(paddr_t paddr)
{
    page_t *page = paddr_to_page(paddr);
    page_free(page);
}

void *alloc_page()
{
    paddr_t paddr = alloc_frame();
    if (paddr == 0)
        return NULL;
    return (void *) to_vaddr(paddr);
}

void free_page(void *pointer)
{
    page_t *page = vaddr_to_page((vaddr_t)pointer);
    page_free(page);
}
