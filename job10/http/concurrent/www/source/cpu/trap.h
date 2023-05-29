#ifndef _CPU_TRAP_H
#define _CPU_TRAP_H

typedef struct regs {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint16_t gs, __gs;
    uint16_t fs, __fs;
    uint16_t es, __es;
    uint16_t ds, __ds;
    uint32_t eax;
    uint32_t error_code;  /* for interrupt and system call, this field is 0 */
    uint32_t eip;
    uint16_t cs, __cs;
    uint32_t eflags;
    uint32_t esp;
    uint16_t ss, __ss;
} regs_t;;

extern void process_entry();
extern void init_idle_regs(regs_t *regs);

#endif
