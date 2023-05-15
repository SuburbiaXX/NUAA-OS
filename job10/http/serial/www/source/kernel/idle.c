#include <utils/root.h>
#include <mm/memory.h>
#include <cpu/mmu.h>
#include <cpu/pmode.h>
#include <cpu/irq.h>
#include <cpu/cpu.h>
#include <cpu/trap.h>
#include "pcb.h"
#include "idle.h"

/**
 * scripts/idle.sh will generate machine code for idle.asm
 * 
 *         %include "sys_number.asm"
 * 
 *         global _start
 *         org 0x1000
 *         [bits 32]
 *         section .text
 * _start:
 *         mov     eax, __SYS_fork
 *         int     0x80
 *         cmp     eax, 0
 *         jz      child
 * 
 *         ; The idle process (pid == 0)
 * parent: 
 *         mov     eax, __SYS_nice   
 *         int     0x80
 *         jmp     parent
 * 
 *         ; The init process (pid == 1)
 * child:
 *         mov     eax, __SYS_setup
 *         int     0x80
 *         mov     eax, __SYS_execve
 *         mov     ebx, path
 *         mov     ecx, argv
 *         mov     edx, envv
 *         int     0x80
 *         jmp     $
 * 
 * path:   db      "/sbin/init", 0 
 * arg0:   db      "init", 0 
 * argv:   dd      arg0, 0 
 * env0:   db      "PATH=/bin:/sbin", 0
 * envv:   dd      env0, 0   
 */
unsigned char idle_machine_code[] = {
  0xb8, 0x02, 0x00, 0x00, 0x00, 0xcd, 0x80, 0x83, 0xf8, 0x00, 0x74, 0x09,
  0xb8, 0x22, 0x00, 0x00, 0x00, 0xcd, 0x80, 0xeb, 0xf7, 0xb8, 0x00, 0x00,
  0x00, 0x00, 0xcd, 0x80, 0xb8, 0x03, 0x00, 0x00, 0x00, 0xbb, 0x34, 0x10,
  0x00, 0x00, 0xb9, 0x44, 0x10, 0x00, 0x00, 0xba, 0x5c, 0x10, 0x00, 0x00,
  0xcd, 0x80, 0xeb, 0xfe, 0x2f, 0x73, 0x62, 0x69, 0x6e, 0x2f, 0x69, 0x6e,
  0x69, 0x74, 0x00, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x3f, 0x10, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x50, 0x41, 0x54, 0x48, 0x3d, 0x2f, 0x62, 0x69,
  0x6e, 0x3a, 0x2f, 0x73, 0x62, 0x69, 0x6e, 0x00, 0x4c, 0x10, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

static void init_fs(pcb_t *pcb)
{
    int i;

    pcb->umask = 0;
    pcb->root_dir = NULL;
    pcb->work_dir = NULL;
    for (i = 0; i < OPEN_MAX; i++)
        pcb->file_table[i] = NULL;
}

static void init_vm(pcb_t *pcb)
{
    pgtbl_t *pgtbl;
    segment_t *segment;
    pte_t *pte;
    void *page;

    pgtbl = pgtbl_new();
    kernel_pgtbl_share(pgtbl);
    pcb->pgtbl = pgtbl;
    pcb->kernel_stack = alloc_page();

    segment = &pcb->code_segment;
    segment->start = IDLE_CODE_START;
    segment->end = IDLE_CODE_END;
    map_segment(pgtbl, segment, PTE_USER|PTE_WRITABLE|PTE_PRESENT);

    segment = &pcb->data_segment;;
    segment->start = IDLE_DATA_START;
    segment->end = IDLE_DATA_END;
    map_segment(pgtbl, segment, PTE_USER|PTE_WRITABLE|PTE_PRESENT);

    segment = &pcb->stack_segment;
    segment->start = IDLE_STACK_START;
    segment->end = IDLE_STACK_END;
    map_segment(pgtbl, segment, PTE_USER|PTE_WRITABLE|PTE_PRESENT);

    pte = pgtbl_open_pte(pgtbl, IDLE_CODE_START);
    page = (void *)pte_vaddr(pte);
    memcpy(page, idle_machine_code, sizeof(idle_machine_code));
}

void launch_idle_process()
{
    pcb_t dummy_pcb;
    pcb_t *pcb;
    regs_t *regs;
    
    pcb = install_pcb(-1);
    strcpy(pcb->name, "idle");
    init_fs(pcb);
    init_vm(pcb);

    pcb->signal_set = 0;
    pcb->exit_code = 0;
    cond_init(&pcb->wait_child_exit);

    context_init(&pcb->context, pcb->kernel_stack);
    regs = (regs_t *)pcb->context.esp;
    init_idle_regs(regs);

    puts("init idle process");
    this_pcb = &dummy_pcb;
    switch_to(pcb);
}
