#ifndef _CPU_IRQ_H
#define _CPU_IRQ_H

#define IRQ_NUMBER 16
#define IRQ_CLOCK  0 
#define IRQ_KBD    1    
#define IRQ_HD     14

typedef void (*isr_t)(int irq_no);
extern void register_irq(int irq_no, isr_t isr);
extern void register_soft_irq(int irq_no, isr_t isr);

extern void disable_all_irq();
extern void enable_all_irq();
extern void enable_irq(int irq_no);

#define M8259_EOI   0x20
#define M8259_IMR   0x21
#define S8259_EOI   0xa0
#define S8259_IMR   0xa1
#define EOI_VALUE   0x20

#endif
