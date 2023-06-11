#ifndef _CPU_H
#define _CPU_H

/* 
 * On x86-32/linux:
 * Scratch register: EAX ECX EDX ST(0)-ST(7) XMM(0)-XMM(7) YMM(0)-YMM(7)
 * Callee saved register: EBX ESI EDI EBP
 *
 * On x86-64/linux:
 * Scratch register: RAX RCX RDX RSI RDI R8-R11
 *                   ST(0)-ST(7) XMM(0)-XMM(15) YMM(0)-YMM(15)
 * Callee saved register: RBX RBP R12-R15
 */

#include <stdint.h>

typedef struct {       // offset
    uint64_t rip;      // 0
    uint64_t rsp;      // 8
    uint64_t rbp;      // 16
    uint64_t rbx;      // 24
    uint64_t r12;      // 32
    uint64_t r13;      // 40
    uint64_t r14;      // 48
    uint64_t r15;      // 56
} cpu_t;

static inline void cpu_init(cpu_t *cpu, uint64_t pc, uint64_t sp)
{
    cpu->rip = pc;
    cpu->rsp = sp;
}

extern uint64_t cpu_switch(cpu_t *source, cpu_t *target, uint64_t value);

#endif
