#ifndef _KERNEL_PCB_H
#define _KERNEL_PCB_H

#include <mm/vm.h>
#include <cpu/cpu.h>
#include "limits.h"
#include "sync.h"

struct pgtbl;
struct file;
struct inode;

enum {
    PCB_RUNNING,
    PCB_WAIT,
    PCB_ZOMBIE,
};

typedef struct pcb {
    // schedule infomation
    chain_t chain;
    int state;

    // id infomation
    char name[NAME_MAX];
    pid_t pid;
    pid_t ppid;
    uid_t uid, euid;
    gid_t gid, egid;

    // memory infomation
    struct pgtbl *pgtbl;
    char *kernel_stack;
    segment_t segment_array[0];
    segment_t code_segment;
    segment_t data_segment;
    segment_t stack_segment;

    // file system inforamtion
    ushort umask;
    struct inode *root_dir, *work_dir;
    struct file *file_table[OPEN_MAX];

    // signal information
    int signal_set;
    int exit_code;
    cond_t wait_child_exit;

    // hardware context
    context_t context;
} pcb_t;

#define SEGMENT_NUMBER 3

#define PCB_NUMBER 128
extern list_t running_pcb_list;
extern pcb_t *pcb_table[PCB_NUMBER];
extern pcb_t *this_pcb;
extern pcb_t *find_pcb(pid_t pid);
extern pid_t alloc_pid();
extern pcb_t *install_pcb(pid_t ppid);
extern void uninstall_pcb(pcb_t *pcb);

extern int super_user();
extern int has_signal();

#endif
