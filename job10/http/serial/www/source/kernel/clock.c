#include <utils/root.h>
#include <cpu/io.h>
#include <cpu/irq.h>
#include "pcb.h"

#define I8253_CLOCK0        0x40
#define I8253_MODE          0x43
#define I8253_FREQ          1193180

#define CLOCKS_PER_SECOND   100
#define LATCH               (I8253_FREQ/CLOCKS_PER_SECOND)

ulong passed_clocks;
int need_reschedule;

void clock_isr(int irq_no)
{
}

void clock_soft_isr(int irq_no)
{
    passed_clocks++;
    schedule();
}

void clock_init()
{
    int lo = LATCH & 0xFF;
    int hi = LATCH >> 8;

    puts("init clock");
    out_byte(I8253_MODE, 0x36);  // binary, mode 3, LSB/MSB, ch 0
    out_byte(I8253_CLOCK0, lo);
    out_byte(I8253_CLOCK0, hi);
    register_irq(IRQ_CLOCK, clock_isr);
    register_soft_irq(IRQ_CLOCK, clock_soft_isr);
}
