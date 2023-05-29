#include <utils/root.h>
#include <mm/vm.h>
#include <mm/page.h>
#include <mm/memory.h>
#include <fs/inode.h>
#include <kernel/file.h>
#include "pcb.h"
#include "wait.h"
#include "fs/fs.h"

void kill_zombie(pcb_t *pcb)
{
    pgtbl_delete(pcb->pgtbl);
    free_page(pcb->kernel_stack);
    uninstall_pcb(pcb);
    free_memory(pcb);
}

static pcb_t *find_zombie()
{
    pid_t ppid = this_pcb->pid;
    int i;
    
    for (i = 0; i < PCB_NUMBER; i++) {
        pcb_t *pcb = pcb_table[i];
        if (pcb && pcb->ppid == ppid && pcb->state == PCB_ZOMBIE)
            return pcb;
    }

    return NULL;
}

int sys_wait(int *status)
{
    int error;

    if (error = verify_write(status, sizeof(int)))
        return error;

    while (1) {
        pcb_t *zombie = find_zombie();
        if (zombie) {
            *status = zombie->exit_code;
            kill_zombie(zombie);
            return 0;
        }     
        cond_wait(&this_pcb->wait_child_exit);
    }
    return 0;
}

int sys_exit(int status)
{
    pcb_t *that_pcb;

    this_pcb->exit_code = WEXITSTATUS(status);
    close_all_file();
    inode_free(this_pcb->root_dir);
    inode_free(this_pcb->work_dir);
    flush_vm();

    that_pcb = find_pcb(this_pcb->ppid);
    if (that_pcb) {
        signal = 1;
        cond_signal(&that_pcb->wait_child_exit);
    }
    turn_into_zombie();
    return 0;
}
