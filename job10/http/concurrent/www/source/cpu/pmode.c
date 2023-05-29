#include <utils/root.h>
#include <kernel/pcb.h>
#include <kernel/idle.h>
#include "cpu.h"
#include "io.h"
#include "pmode.h"
#include "mmu.h"
#include "trap.h"

gdt_t gdt;
gdt_desc_t gdt_desc = {sizeof(gdt_t)-1, (uint32_t) &gdt};
idt_t idt;
idt_desc_t idt_desc = {sizeof(idt_t)-1, (uint32_t) &idt};
ldt_t ldt;
tss_t tss;

void segment_desc_init_g0(segment_desc_t *this, int attr, 
                          uint32_t base, uint32_t limit)
{
    this->attr = attr;
    this->base_15_00 = base & 0xffff; 
    this->base_23_16 = (base >> 16) & 0xff;
    this->base_31_24 = base >> 24;
    this->limit_15_00 = limit & 0xffff;
    this->limit_19_16 = 0 | (limit >> 16); // g = 0, d = 0
}

void segment_desc_init_g1(segment_desc_t *this, int attr, 
                          uint32_t base, uint32_t limit)
{
    this->attr = attr;
    this->base_15_00 = base & 0xffff; 
    this->base_23_16 = (base >> 16) & 0xff;
    this->base_31_24 = base >> 24;
    this->limit_15_00 = limit & 0xffff;
    this->limit_19_16 = 0xc0 | (limit >> 16); // g = 1, d = 1 
}

void gate_desc_init(gate_desc_t *this, int attr,
                    uint16_t selector, uint32_t offset)
{
    this->attr = attr;
    this->selector = selector;
    this->offset_15_00 = offset & 0xffff;
    this->offset_31_16 = offset >> 16;
    this->dword_count = 0;
}

void install_tss(int index, uint32_t base)
{
}

void gdt_init()
{
    segment_desc_t *desc;

    // null segment 
    desc = gdt.desc + 0;
    memset(desc, 0, 8);

    // kernel code segment 
    desc = gdt.desc + KERNEL_CODE_INDEX;
    segment_desc_init_g1(desc, ATTR_KERNEL_CODE, 0, 0xfffff);

    // kernel data segment
    desc = gdt.desc + KERNEL_DATA_INDEX;
    segment_desc_init_g1(desc, ATTR_KERNEL_DATA, 0, 0xfffff);

    // user code segment
    desc = gdt.desc + USER_CODE_INDEX;
    segment_desc_init_g1(desc, ATTR_USER_CODE, 0, 0xfffff);

    // user data segment
    desc = gdt.desc + USER_DATA_INDEX;
    segment_desc_init_g1(desc, ATTR_USER_DATA, 0, 0xfffff);

    // ldt segment
    desc = gdt.desc + LDT_INDEX;
    segment_desc_init_g0(desc, ATTR_LDT, (uint32_t)&ldt, sizeof(ldt_t)-1);

    // tss segment
    desc = gdt.desc + TSS_INDEX;
    segment_desc_init_g0(desc, ATTR_AVL386TSS, (uint32_t)&tss, sizeof(tss_t)-1);
}

void ldt_init()
{
    segment_desc_t *desc;

    // null segment 
    desc = ldt.desc + LDT_NULL_INDEX;
    memset(desc, 0, 8);
}

void tss_init()
{
    tss.cs = tss.ds = tss.es = tss.ss = tss.fs = tss.gs = GDT_NULL_SELECTOR;

    tss.eax = 0;
    tss.ebx = 0;
    tss.ecx = 0;
    tss.edx = 0;
    tss.esi = 0;
    tss.edi = 0;
    tss.esp = 0;
    tss.ebp = 0;
    tss.eflags = 0;
    tss.eip = 0;

    tss.cr3 = 0;
    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss.esp0 = 0;

    tss.back_link = 0;
    tss.ldt = LDT_SELECTOR;
    tss.trace_bitmap = 0x80000000;
}

void install_trap_handler(int trap_no, int attr, void (*handler)())
{
    gate_desc_t *desc = idt.desc + trap_no;
    gate_desc_init(desc, attr, KERNEL_CODE_SELECTOR, (uint32_t)handler);
}

#define IRQ0 32
#define IRQ_SLAVE 2
    
#define PIC1 0x20
#define PIC2 0xA0
    
#define outb out_byte

void setup_8259()
{
    int master_irq = 0x20;
    int slave_irq = 0x28;

    out_byte(0x20, 0x11);       // initialization sequence 
    out_byte(0xA0, 0x11);

    out_byte(0x21, master_irq); // start of hardware int's (0x20)
    out_byte(0xA1, slave_irq);  // start of hardware int's (0x28)

    out_byte(0x21, 0x04);       // 8259-1 is master 
    out_byte(0xA1, 0x02);       // 8259-2 is slave 

    out_byte(0x21, 0x01);       // 8086 mode for both
    out_byte(0xA1, 0x01);

    out_byte(0x21, 0xff);       // mask off all the interrupt 
    out_byte(0xA1, 0xff);
}

char *trap_info_table[] = {
    /* 0x00 */ "divide error",
    /* 0x01 */ "debug",
    /* 0x02 */ "donmi",
    /* 0x03 */ "int3",
    /* 0x04 */ "overflow",
    /* 0x05 */ "bounds",
    /* 0x06 */ "invalid operand",
    /* 0x07 */ "device not available",
    /* 0x08 */ "double fault",
    /* 0x09 */ "coprocessor segment overrun",
    /* 0x0a */ "invalid tss",
    /* 0x0b */ "segment not present",
    /* 0x0c */ "stack fault",
    /* 0x0d */ "general protection falut",
    /* 0x0e */ "pagefault",
    /* 0x0f */ "intel reserved",
    /* 0x10 */ "invalid TSS",
    /* 0x11 */ "segment not present",
    /* 0x12 */ "stack segment"
};

extern int sys_exit(int status);

/**
 +-----+-----+------------------------------------------------------------+
 |Field|Value|                         Description                        |
 |-----+-----+------------------------------------------------------------|
 | U/S |  0  | The access causing the fault originated when the processor |
 |     |     | was executing in supervisor mode.                          |
 |     |  1  | The access causing the fault originated when the processor |
 |     |     | was executing in user mode.                                |
 |     |     |                                                            |
 | W/R |  0  | The access causing the fault was a read.                   |
 |     |  1  | The access causing the fault was a write.                  |
 |     |     |                                                            |
 | P   |  0  | The fault was caused by a not-present page.                |
 |     |  1  | The fault was caused by a page-level protection violation. |
 +-----+-----+------------------------------------------------------------+
**/
void handle_trap(int trap_no, int ebx)
{
    regs_t *regs = (regs_t *) &ebx;

    printf("trap_no = %x\n", trap_no);
    printf("info    = %s\n", trap_info_table[trap_no]);
    printf("pid     = %x\n", this_pcb->pid);
    printf("cs      = %x\n", regs->cs);
    printf("eip     = %x\n", regs->eip);
    printf("esp     = %x\n", regs->esp);

    uint error = regs->error_code;
    printf("error   = %x, user = %d, writable = %d, present = %d\n", error, 
            error & PTE_USER, error & PTE_WRITABLE, error & PTE_PRESENT);

    uint cr2;
    asm volatile("movl %%cr2,%0":"=r"(cr2):);
    printf("cr2     = %x\n", cr2);

    if (error & PTE_USER) 
        sys_exit(-1);
    for (;;);
}

#define install_trap(no)                                                 \
    extern void trap_##no();                                             \
    install_trap_handler(no, ATTR_386TRAP, trap_##no)

#define install_irq(no)                                                  \
    extern void trap_##no();                                             \
    install_trap_handler(no, ATTR_386INTR, trap_##no)

void idt_init()
{
    memset(&idt, 0, sizeof(idt));

    install_trap(0x00);
    install_trap(0x01);
    install_trap(0x02);
    install_trap(0x03);
    install_trap(0x04);
    install_trap(0x05);
    install_trap(0x06);
    install_trap(0x07);
    install_trap(0x08);
    install_trap(0x09);
    install_trap(0x0a);
    install_trap(0x0b);
    install_trap(0x0c);
    install_trap(0x0d);
    install_trap(0x0e);
    install_trap(0x0f);
    install_trap(0x10);
    install_trap(0x11);
    install_trap(0x12);

    install_irq(0x20);
    install_irq(0x21);
    install_irq(0x22);
    install_irq(0x23);
    install_irq(0x24);
    install_irq(0x25);
    install_irq(0x26);
    install_irq(0x27);
    install_irq(0x28);
    install_irq(0x29);
    install_irq(0x2a);
    install_irq(0x2b);
    install_irq(0x2c);
    install_irq(0x2d);
    install_irq(0x2e);
    install_irq(0x2f);

    extern void trap_0x80();
    install_trap_handler(0x80, ATTR_SYSTEM_CALL, trap_0x80);
}

void context_init(context_t *this, void *kernel_stack)
{
    memset(this, 0, sizeof(context_t));
    this->esp = (vaddr_t)(kernel_stack + PAGE_SIZE - sizeof(regs_t));
    this->eip = (vaddr_t)process_entry; // defined in trap.asm
}

void regs_dump(regs_t *this)
{
    printf("ds = %x\n", this->ds);
    printf("es = %x\n", this->es);
    printf("eip = %x\n", this->eip);
    printf("cs = %x\n", this->cs);
    printf("eflags = %x\n", this->eflags);
    printf("esp = %x\n", this->esp);
    printf("ss = %x\n", this->ss);
}

void dump(ulong ebx)
{
    regs_t *regs = (regs_t *)&ebx;

    regs_dump(regs);
}

void init_idle_regs(regs_t *regs)
{
    regs->ds = regs->es = regs->ss = regs->fs = regs->gs = USER_DATA_SELECTOR;
    regs->eip = IDLE_CODE_START;
    regs->cs = USER_CODE_SELECTOR;
    regs->eflags = EFLAG_IF;
    regs->esp = IDLE_STACK_END;
    regs->ss = USER_DATA_SELECTOR;
}

void switch_to(pcb_t *that_pcb)
{
    context_t *source = &this_pcb->context;
    context_t *target = &that_pcb->context;

    this_pcb = that_pcb;
    load_pgtbl(that_pcb->pgtbl);
    tss.esp0 = (vaddr_t)(that_pcb->kernel_stack + PAGE_SIZE);
    switch_context(source, target);
}
