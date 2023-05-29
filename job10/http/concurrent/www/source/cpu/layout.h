#ifndef _CPU_LAYOUT_H
#define _CPU_LAYOUT_H

#define PHYSICAL_MEMORY (4 * 1024 * 1024)
#define PHYSICAL_PAGES (PHYSICAL_MEMORY/PAGE_SIZE)

#define USER_START              0
#define USER_END                (3*G)
#define KERNEL_START            USER_END
#define KERNEL_END              (KERNEL_START + PHYSICAL_MEMORY)

extern char __text_start, __text_end; 
extern char __data_start, __data_end;
extern char __bss_start, __bss_end;

#define to_vaddr(paddr) ((ulong)(paddr) + KERNEL_START)
#define to_paddr(vaddr) ((ulong)(vaddr) - KERNEL_START)
#define in_kerel_space(vaddr) (((ulong)(vaddr)) >= USER_END)
#define in_user_space(vaddr) (((ulong)(vaddr)) < USER_END)

#endif
