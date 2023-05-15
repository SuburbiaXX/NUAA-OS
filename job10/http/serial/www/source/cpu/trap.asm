%define KERNEL_DATA_INDEX        2
%define KERNEL_DATA_SELECTOR     (KERNEL_DATA_INDEX << 3)
%define ERROR_CODE 0
%define EAX 40

%macro push_regs 0
        ; save registers
        push    ds
        push    es
        push    fs
        push    gs
        push    ebp
        push    edi
        push    esi
        push    edx
        push    ecx
        push    ebx

        ; load ds/es
        push    KERNEL_DATA_SELECTOR
        pop     ds
        push    KERNEL_DATA_SELECTOR
        pop     es
%endmacro

%macro pop_regs 0
        ; restore registers 
        pop     ebx
        pop     ecx
        pop     edx
        pop     esi
        pop     edi
        pop     ebp
        pop     gs
        pop     fs
        pop     es
        pop     ds
%endmacro

%macro trap0 2                  ; trap without error code
        global  %1
%1:
        push    ERROR_CODE      ; push a dummy error code
        push    %2              ; push trap_no
        jmp     trap_00_20
%endmacro

%macro trap1 2                  ; trap with error code
        global  %1
%1:
                                ; error code has been pushed
        push    %2              ; push trap_no
        jmp     trap_00_20
%endmacro

%macro trap2 2                  ; irq handler
        global  %1
%1:
        push    ERROR_CODE      ; push a dummy error code
        push    %2              ; push trap_no
        jmp     trap_20_2f
%endmacro

        extern handle_trap
        extern handle_irq
        extern handle_soft_irq
        global process_entry
        section .text

process_entry:
        pop_regs
        pop     eax
        add     esp, 4          ; pop up error code
        iret

trap_00_20:
        cli
        xchg    eax, [esp]      ; eax <--> trap_no
        push_regs

        push    eax             ; push trap_no
        call    handle_trap
        add     esp, 4
        call    handle_soft_irq

        pop_regs
        pop     eax
        add     esp, 4          ; pop up error code
        iret

trap_20_2f:
        cli
        xchg    eax, [esp]      ; eax <--> trap_no
        push_regs

        sub     eax, 0x20       ; convert trap_no to irq_no
        push    eax             ; push irq_no
        call    handle_irq
        add     esp, 4
        call    handle_soft_irq

        pop_regs
        pop     eax
        add     esp, 4          ; pop up error code
        iret

        global  trap_0x80
trap_0x80:
        push    ERROR_CODE      ; push a dummy error code
        push    eax             ; push the original eax
        push_regs

        call    [sys_call_table + eax*4]
        mov     [esp + EAX], eax ; save the return value
        call    handle_soft_irq

        pop_regs
        pop     eax             ; pop the return value to eax
        add     esp, 4          ; pop the error code
        iret

        trap0   trap_0x00, 0x00
        trap0   trap_0x01, 0x01
        trap0   trap_0x02, 0x02
        trap0   trap_0x03, 0x03
        trap0   trap_0x04, 0x04
        trap0   trap_0x05, 0x05
        trap0   trap_0x06, 0x06
        trap0   trap_0x07, 0x07
        trap1   trap_0x08, 0x08
        trap1   trap_0x09, 0x09
        trap1   trap_0x0a, 0x0a
        trap1   trap_0x0b, 0x0b
        trap1   trap_0x0c, 0x0c
        trap1   trap_0x0d, 0x0d
        trap1   trap_0x0e, 0x0e
        trap1   trap_0x0f, 0x0f
        trap0   trap_0x10, 0x10
        trap1   trap_0x11, 0x11
        trap1   trap_0x12, 0x12

        trap2   trap_0x20, 0x20
        trap2   trap_0x21, 0x21
        trap2   trap_0x22, 0x22
        trap2   trap_0x23, 0x23
        trap2   trap_0x24, 0x24
        trap2   trap_0x25, 0x25
        trap2   trap_0x26, 0x26
        trap2   trap_0x27, 0x27
        trap2   trap_0x28, 0x28
        trap2   trap_0x29, 0x29
        trap2   trap_0x2a, 0x2a
        trap2   trap_0x2b, 0x2b
        trap2   trap_0x2c, 0x2c
        trap2   trap_0x2d, 0x2d
        trap2   trap_0x2e, 0x2e
        trap2   trap_0x2f, 0x2f

%macro sys 1
        extern %1
        dd %1
%endmacro

        align 4
        global sys_call_table
sys_call_table:
        sys     sys_setup       ; 0
        sys     sys_uname       
        sys     sys_fork        
        sys     sys_execve      
        sys     sys_wait        
        sys     sys_exit        ; 5
        sys     sys_open        
        sys     sys_read        
        sys     sys_write       
        sys     sys_lseek       
        sys     sys_ioctl       ; 10
        sys     sys_close       
        sys     sys_creat       
        sys     sys_umask       
        sys     sys_pipe        
        sys     sys_dup         ; 15
        sys     sys_dup2        
        sys     sys_stat        
        sys     sys_chown       
        sys     sys_chmod       
        sys     sys_mkdir       ; 20
        sys     sys_mknod       
        sys     sys_rmdir       
        sys     sys_link        
        sys     sys_unlink      
        sys     sys_rename      ; 25
        sys     sys_chroot      
        sys     sys_chdir       
        sys     sys_getcwd      
        sys     sys_getpid      
        sys     sys_getppid     ; 30
        sys     sys_kill        
        sys     sys_signal      
        sys     sys_brk         
        sys     sys_nice        
        sys     sys_sync        ; 35
        sys     sys_getuid      
        sys     sys_setuid      
        sys     sys_geteuid     
        sys     sys_seteuid     
        sys     sys_getgid      ; 40
        sys     sys_setgid      
        sys     sys_getegid     
        sys     sys_setegid     
        sys     sys_ps
