#define RIP     0
#define RSP     8
#define RBP     16
#define RBX     24
#define R12     32
#define R13     40
#define R14     48
#define R15     56 

    .global cpu_switch          
cpu_switch:                     
    # rdi -> source             
    # rsi -> target             
    # rdx -> vaule              
                                
    popq %rcx                   
    movq %rcx, RIP(%rdi)          
    movq %rsp, RSP(%rdi)          
    movq %rbp, RBP(%rdi)         
    movq %rbx, RBX(%rdi)         
    movq %r12, R12(%rdi)         
    movq %r13, R13(%rdi)         
    movq %r14, R14(%rdi)         
    movq %r15, R15(%rdi)         
                                
    movq RIP(%rsi), %rcx          
    movq RSP(%rsi), %rsp          
    movq RBP(%rsi), %rbp         
    movq RBX(%rsi), %rbx         
    movq R12(%rsi), %r12         
    movq R13(%rsi), %r13         
    movq R14(%rsi), %r14         
    movq R15(%rsi), %r15         
    movq %rdx, %rax             
    jmp *%rcx                   
