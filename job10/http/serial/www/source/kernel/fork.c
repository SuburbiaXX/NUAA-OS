#include <utils/root.h>
#include <mm/page.h>
#include <mm/memory.h>
#include <fs/inode.h>
#include <cpu/trap.h>
#include "errno.h"
#include "pcb.h"
#include "file.h"
#include "fs/fs.h"

void copy_fs(pcb_t *that_pcb)
{
    int i;
    inode_t *inode;
    file_t *file;

    that_pcb->umask = this_pcb->umask;

    inode = that_pcb->root_dir = this_pcb->root_dir;
    if (inode)
        inode_hold(inode);

    inode = that_pcb->work_dir = this_pcb->work_dir;
    if (inode)
        inode_hold(inode);

    for (i = 0; i < OPEN_MAX; i++) {
        if (file = that_pcb->file_table[i] = this_pcb->file_table[i])
            file_hold(file);
    }
}

int sys_fork(ulong ebx)
{
    regs_t *this_regs;
    regs_t *that_regs; 
    pcb_t *that_pcb;

    that_pcb = install_pcb(this_pcb->pid);
    strcpy(that_pcb->name, this_pcb->name);
    copy_vm(that_pcb);
    copy_fs(that_pcb);
    that_pcb->signal_set = 0;
    that_pcb->exit_code = 0;
    cond_init(&that_pcb->wait_child_exit);

    context_init(&that_pcb->context, that_pcb->kernel_stack);
    this_regs = (regs_t *)&ebx;
    that_regs = (regs_t *)that_pcb->context.esp;
    *that_regs = *this_regs;
    that_regs->eax = 0;

    return that_pcb->pid;
}
