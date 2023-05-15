%define CR0_PE		(1 << 0)
%define CR0_MP		(1 << 1)
%define CR0_EM		(1 << 2)
%define CR0_TS		(1 << 3)
%define CR0_WP		(1 << 16)
%define CR0_PG		(1 << 31)

%define PTE_PRESENT     (1 << 0)
%define PTE_WRITE       (1 << 1)
%define OUTTER_PGTBL    0x1000
%define INNER_PGTBL     0x2000

%define READ_DISK       2
%define WRITE_DISK      3
%define DISK_A          0
%define DISK_B          1
%define DISK_C          0x80

%define drive           dl
%define track           ch
%define head            dh
%define sector          cl
%define sectors         al
%define service         ah

%define buffer_segment  es
%define buffer_offset   bx
%define tmp             si
%define total_sectors   di

        ; put char in real mode
%macro putc 1
        mov     ax, 0xB800     
        mov     es, ax
        mov     al, %1
        mov     ah, 0x07 
        mov     [es:0], ax
%endmacro

        ; put char in protect mode
%macro putx 1
        mov     al, %1
        mov     ah, 0x07 
        mov     [0xB8000], ax
%endmacro

        org 0x7c00
        [bits 16]
        ;
        ; First, BIOS loads the bootsector into 0000:7C00.
        ;
        putc    '1'
        cli
        xor     ax, ax
        mov     ds, ax
        mov     ss, ax
        mov     sp, 0

        ;
        ; Then, the bootsector loads kernel into 1000:0000.
        ; The address of kernel entry is 0xC0010000.
        ; 0xC0010000 is hardcoded in scripts/Linked.ld
        ;
        mov     ax, 0x1000
        mov     buffer_segment, ax
        xor     buffer_offset, buffer_offset

        mov     drive, DISK_A
        mov     track, 0
        mov     head, 0
        mov     sector, 2            ; sector count from 1
        mov     total_sectors, 128   ; read 128 sectors, 64K

read_next_sector: 
        mov     sectors, 1
        mov     service, READ_DISK
        int     0x13
        dec     total_sectors
        jz      read_complete

        ; Update buffer segment and offset
        add     buffer_offset, 512

        ; Update sector
        inc     sector
        cmp     sector, 19
        jnz     read_next_sector
        mov     sector, 1           ; sector count from 1

        ; Update head
        inc     head
        cmp     head, 2
        jnz     read_next_sector
        mov     head, 0

        ; Update track
        inc     track
        jmp     read_next_sector
read_complete:

        ;
        ; Enable A20 
        ;
        putc    '2'
wait_8042_1:
        in      al, 0x64 
        test    al, 0x2
        jnz     wait_8042_1
        mov     al, 0xd1
        out     0x64, al 

wait_8042_2:
        in      al, 0x64 
        test    al, 0x2
        jnz     wait_8042_2
        mov     al, 0xdf
        out     0x60, al 

        ;
        ; Switch to protect mode 
        ;
        lgdt    [gdt_desc]
        mov     eax, cr0         
        or      eax, CR0_PE
        mov     cr0, eax
        jmp     0x08:start32

        [bits 32]
start32:
        ;
        ; Initialize inner page table
        ;
        cli
        mov     ax, 0x10
        mov     ds, ax
        mov     es, ax
        mov     ss, ax
        mov     esp, 0x10000
        putx    '3'

        mov     eax, PTE_WRITE|PTE_PRESENT
        mov     edi, INNER_PGTBL
        mov     ecx, 1024
        cld
init_pte:
        stosd
        add     eax, 4096
        loop    init_pte

        ;
        ; Initialize outter page table
        ;
        xor     eax, eax
        mov     edi, OUTTER_PGTBL
        mov     ecx, 1024
        cld
        rep
        stosd
        mov     dword [OUTTER_PGTBL+000*4], INNER_PGTBL|PTE_WRITE|PTE_PRESENT
        mov     dword [OUTTER_PGTBL+768*4], INNER_PGTBL|PTE_WRITE|PTE_PRESENT

        ;
        ; Load CR3
        ;
        mov     eax, OUTTER_PGTBL 
        mov     cr3, eax

        ;
        ; Enable paging and write-protect
        ;
        mov     eax, cr0
        and     eax, ~(CR0_EM|CR0_TS)
        or      eax, CR0_PG|CR0_WP|CR0_MP
        mov     cr0, eax

        ;
        ; Jump to start_kernel
        ;
        putx    '4'
        jmp     0x08:0xC0010000

        align   8                       
gdt:    dw      0,0,0,0         ; dummy
        dw      0xFFFF          ; limit=4GB
        dw      0x0000          ; base address=0
        dw      0x9A00          ; code read/exec
        dw      0x00CF          ; granularity=4096,386
        dw      0xFFFF          ; limit=4GB
        dw      0x0000          ; base address=0
        dw      0x9200          ; data read/write
        dw      0x00CF          ; granularity=4096,386

        align   8
gdt_desc: 
        dw      23              ; gdt limit=sizeof(gdt) - 1
        dw      gdt

        times 510-($-$$) db 0
        dw      0xAA55
