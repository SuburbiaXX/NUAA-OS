#include <utils/root.h>
#include <fs/inode.h>
#include <mm/memory.h>
#include <mm/vm.h>
#include "errno.h"
#include "pcb.h"

pcb_t *pcb_table[PCB_NUMBER];

pcb_t *find_pcb(pid_t pid)
{
    for (int i = 0; i < PCB_NUMBER; i++) {
        pcb_t *pcb = pcb_table[i];
        if (pcb && pcb->pid == pid)
            return pcb;
    }
    return NULL;
}

pid_t alloc_pid()
{
    for (int i = 0; i < PCB_NUMBER; i++) {
        pcb_t *pcb = pcb_table[i];
        if (pcb == NULL)
            return i;
    }
    return EAGAIN;
}

pcb_t *install_pcb(pid_t ppid)
{
    pcb_t *pcb = alloc_memory(sizeof(pcb_t));

    pcb->pid = alloc_pid();
    pcb_table[pcb->pid] = pcb;
    pcb->ppid = ppid;

    chain_init(&pcb->chain);
    list_push_back(&running_pcb_list, pcb);

    pcb->state = PCB_RUNNING;
    pcb->uid = pcb->euid = 0;
    pcb->gid = pcb->egid = 0;
    return pcb;
}

void uninstall_pcb(pcb_t *pcb)
{
    pcb_table[pcb->pid] = NULL;
    chain_remove(pcb);
}

int has_signal()
{
    return this_pcb->signal_set != 0;
}

int super_user()
{
    return this_pcb->uid == 0;
}

int access_inode(inode_t *this, int mask)
{
    int mode = this->mode;

    if (this_pcb->euid == this->uid)
        mode >>= 6;
    else if (this_pcb->egid == this->gid)
        mode >>= 3;
    return super_user() || ((mode & mask & 7) == mask);
}

int sys_getpid()
{
    return this_pcb->pid;
}

int sys_getppid()
{
    return this_pcb->ppid;
}

int sys_getuid()
{
    return this_pcb->uid;
}

int sys_setuid(uid_t uid)
{
    if (super_user()) {
        this_pcb->uid = uid;
        this_pcb->euid = uid;
        return 0;
    }
    return EPERM;
}

int sys_getgid()
{
    return this_pcb->gid;
}

int sys_setgid(gid_t gid)
{
    if (super_user()) {
        this_pcb->gid = gid;
        this_pcb->egid = gid;
        return 0;
    }
    return EPERM;
}

int sys_geteuid()
{
    return this_pcb->euid;
}

int sys_seteuid(uid_t euid)
{
    if (super_user()) {
        this_pcb->euid = euid;
        return 0;
    }
    return EPERM;
}

int sys_getegid()
{
    return this_pcb->egid;
}

int sys_setegid(gid_t egid)
{
    if (super_user()) {
        this_pcb->egid = egid;
        return 0;
    }
    return EPERM;
}

int pcb_dump(pcb_t *this, char *stream)
{
    char *state = NULL;
    switch (this->state) {
        case PCB_RUNNING:
            state = "RUNNING";
            break;

        case PCB_WAIT:
            state = "WAIT";
            break;

        case PCB_ZOMBIE:
            state = "ZOMBIE";
            break;
    }
    int len = sprintf(stream, "%d\t%d\t%s\t%s\n", 
                      this->pid, this->ppid, this->name, state);
    return len;
}

int sys_ps(void *buf, int size)
{
    int error = verify_write(buf, size);
    if (error < 0)
        return error;

    void *p = buf;
    p += sprintf(p, "PID\tPPID\tCMD\tSTATE\n");
    for (int i = 0; i < PCB_NUMBER; i++) {
        pcb_t *pcb = pcb_table[i];
        if (pcb)
            p += pcb_dump(pcb, p);
    } 
    return p - buf;
}
