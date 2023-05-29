#ifndef _CPU_PROTECT_MODE_H
#define _CPU_PROTECT_MODE_H

typedef struct tss {
    uint32_t back_link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t trace_bitmap;
} tss_t;
extern tss_t tss;

// segment attr byte 
#define ATTR_USER_CODE        0xfa
#define ATTR_USER_DATA        0xf2
#define ATTR_KERNEL_CODE      0x9a
#define ATTR_KERNEL_DATA      0x92
#define ATTR_LDT              0x82
#define ATTR_AVL386TSS        0x89
#define ATTR_386INTR          0x8e
#define ATTR_386TRAP          0x8f
#define ATTR_SYSTEM_CALL      0xef

typedef struct {
    uint16_t limit_15_00;
    uint16_t base_15_00;
    uint8_t base_23_16;
    uint8_t attr; 
    uint8_t limit_19_16;
    uint8_t base_31_24;
}  gcc_packed segment_desc_t;

typedef struct {
    uint16_t offset_15_00;
    uint16_t selector;
    uint8_t dword_count;
    uint8_t attr;
    uint16_t offset_31_16;
}  gcc_packed gate_desc_t;

// GDT definition
#define GDT_NULL_INDEX           0
#define KERNEL_CODE_INDEX        1
#define KERNEL_DATA_INDEX        2
#define USER_CODE_INDEX          3
#define USER_DATA_INDEX          4
#define LDT_INDEX                5
#define TSS_INDEX                6

#define GDT_NULL_SELECTOR        (GDT_NULL_INDEX << 3)
#define KERNEL_CODE_SELECTOR     (KERNEL_CODE_INDEX << 3)
#define KERNEL_DATA_SELECTOR     (KERNEL_DATA_INDEX << 3)
#define USER_CODE_SELECTOR       ((USER_CODE_INDEX << 3) + 3)
#define USER_DATA_SELECTOR       ((USER_DATA_INDEX << 3) + 3)
#define LDT_SELECTOR             (LDT_INDEX << 3)
#define TSS_SELECTOR             (TSS_INDEX << 3)

typedef struct {
    segment_desc_t desc[7];
} gcc_aligned(8) gdt_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} gcc_packed gdt_desc_t ;

// LDT definition
#define LDT_NULL_INDEX           0
#define LDT_NULL_SELECTOR        (LDT_NULL_INDEX << 3)

typedef struct {
    segment_desc_t desc[1];
} gcc_aligned(8) ldt_t;

// IDT definition
typedef struct {
    gate_desc_t desc[256];
} gcc_aligned(8) idt_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} gcc_packed idt_desc_t;

struct pcb;
extern void switch_to(struct pcb *that_pcb);
#endif
