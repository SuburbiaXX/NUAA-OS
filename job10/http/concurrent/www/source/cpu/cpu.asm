%define KERNEL_START 0xC0000000

        global load_pgtbl
        global cli
        global sti
        global tlb_flush_all
        global tlb_flush_page
        global switch_context

	section .text
cli:
        cli
        ret

sti:
        sti
        ret

tlb_flush_all:
        mov     eax, cr3
        mov     cr3, eax
        ret

tlb_flush_page:
        mov     eax, [esp + 4]
        invlpg  [eax]
        ret

load_pgtbl:
        mov     eax, [esp + 4]
        sub     eax, KERNEL_START
        mov     cr3, eax 
        jmp     flush
flush:
        ret

%define EBX 0
%define ESI 4
%define EDI 8
%define EBP 12
%define ESP 16
%define EIP 20

switch_context:
        mov     eax, [esp + 4]          ; eax = source context
        mov     edx, [esp + 8]          ; edx = target context
        pop     ecx                     ; ecx = return address

        mov     [eax + EBX], ebx
        mov     [eax + ESI], esi
        mov     [eax + EDI], edi
        mov     [eax + EBP], ebp
        mov     [eax + ESP], esp
        mov     [eax + EIP], ecx

        mov     ebx, [edx + EBX]
        mov     esi, [edx + ESI]
        mov     edi, [edx + EDI]
        mov     ebp, [edx + EBP]
        mov     esp, [edx + ESP]
        mov     ecx, [edx + EIP]        ; ecx = return address
        push    ecx                     ; push eip onto target stack
        ret
