#include <utils/root.h>
#include "io.h" 
#include "irq.h"
#include "trap.h"
#include "pmode.h"
#include "cpu.h"

volatile long irq_flags;
isr_t isr_table[IRQ_NUMBER];
isr_t soft_isr_table[IRQ_NUMBER];

void register_irq(int irq_no, isr_t isr)
{
    isr_table[irq_no] = isr;
}

void register_soft_irq(int irq_no, isr_t soft_isr)
{
    soft_isr_table[irq_no] = soft_isr;
}

int test_irq(int irq_no)
{
    return irq_flags & (1 << irq_no);
}

void clear_irq(int irq_no)
{
    irq_flags &= ~(1 << irq_no);
}

void handle_irq(int irq_no)
{
    assert(irq_no >= 0 && irq_no < 16);

    irq_flags |= (1 << irq_no);
    isr_t isr = isr_table[irq_no];
    isr(irq_no);

    if (irq_no < 8) {
        out_byte(M8259_EOI, EOI_VALUE);
    } else {
        out_byte(S8259_EOI, EOI_VALUE);
        out_byte(M8259_EOI, EOI_VALUE);
    }
}

void handle_soft_irq(ulong ebx)
{
    regs_t *regs = (regs_t *)&ebx;
    if (regs->cs != USER_CODE_SELECTOR)
        return;
    if (!irq_flags)
        return;

    for (int i = 0; i < IRQ_NUMBER; i++) {
        if (test_irq(i)) {
            isr_t soft_isr = soft_isr_table[i];
            if (soft_isr)
                soft_isr(i);
            clear_irq(i);
        }
    }
}

void enable_all_irq()
{
    out_byte(M8259_IMR, 0);
    out_byte(S8259_IMR, 0);
}

void disable_all_irq()
{
    out_byte(M8259_IMR, 0xff);
    out_byte(S8259_IMR, 0xff);
}

/* enable INT when the bit is off */
#define mask(irq_no) (~(1 << (irq_no)))

void enable_irq(int irq_no)
{
    if (irq_no < 8) {
        out_byte(M8259_IMR, in_byte(M8259_IMR) & mask(irq_no));
    } else {
        out_byte(M8259_IMR, in_byte(M8259_IMR) & mask(2));
        out_byte(S8259_IMR, in_byte(S8259_IMR) & mask(irq_no & 7));
    }
}

#undef mask
