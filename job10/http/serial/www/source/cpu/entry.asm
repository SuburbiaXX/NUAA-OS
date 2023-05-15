%define KERNEL_START             0xC0000000
%define KERNEL_CODE_INDEX        1
%define KERNEL_DATA_INDEX        2
%define LDT_INDEX                5
%define TSS_INDEX                6

%define KERNEL_CODE_SELECTOR     (KERNEL_CODE_INDEX << 3)
%define KERNEL_DATA_SELECTOR     (KERNEL_DATA_INDEX << 3)
%define LDT_SELECTOR             (LDT_INDEX << 3)
%define TSS_SELECTOR             (TSS_INDEX << 3)

        extern setup_8259
        extern idt_init
        extern idt_desc
        extern gdt_init
        extern gdt_desc
        extern ldt_init
        extern tss_init
        extern __bss_start
        extern __bss_end
        extern main
        global start_kernel

        section .text 
start_kernel:
        cli
        mov     esp, KERNEL_START + 0x10000

        ; Initialize bss
        mov     ecx, __bss_end
        sub     ecx, __bss_start
        mov     edi, __bss_start
        xor     eax, eax
        rep
        stosb   

        ; Initialize gdt
        call    gdt_init
        lgdt    [gdt_desc]

        ; Initialize ldt
        call    ldt_init
        mov     ax, LDT_SELECTOR
        lldt    ax

        ; Initialize tss
        call    tss_init
        mov     ax, TSS_SELECTOR
        ltr     ax

        ; Initialize cs/ds/es/ss
        jmp     KERNEL_CODE_SELECTOR:load_cs
load_cs:
        mov     eax, KERNEL_DATA_SELECTOR
        mov     ds, eax
        mov     es, eax
        mov     ss, eax
        mov     esp, KERNEL_START + 0x10000

        ; Initialize idt
        call    setup_8259    
        call    idt_init
        lidt    [idt_desc]

        ; Jump to C
        jmp     main
