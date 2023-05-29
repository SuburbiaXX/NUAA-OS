#ifndef _MM_VM_H
#define _MM_VM_H

#include <cpu/mmu.h>
extern void kernel_pgtbl_init();
extern void kernel_pgtbl_share(pgtbl_t *pgtbl);

struct inode;
typedef struct {
    ulong start;
    ulong end;
} segment_t;

struct pcb;
extern void map_segment(pgtbl_t *pgtbl, segment_t *segment, int protect);
extern void copy_vm(struct pcb *that_pcb);
extern void flush_vm();

extern int verify_read(void *buffer, size_t size);
extern int verify_write(void *buffer, size_t size);
extern int verify_string(char *string);
extern int verify_vector(char *vector[]);
extern int verify_path(char *path);

#endif
