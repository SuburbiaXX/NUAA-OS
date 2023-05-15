#include <utils/root.h>
#include <mm/page.h>
#include <mm/ustack.h>
#include <mm/vm.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <fs/buffer.h>
#include <cpu/trap.h>
#include "pcb.h"
#include "errno.h"
#include "elf.h"

int check_elf(inode_t *inode, elf_head_t *elf_head)
{
    if (inode->size < sizeof(elf_head_t))
        return ENOEXEC;

    if ((elf_head->ident[EI_MAG0] != ELF_MAG0) ||
        (elf_head->ident[EI_MAG1] != ELF_MAG1) ||
        (elf_head->ident[EI_MAG2] != ELF_MAG2) ||
        (elf_head->ident[EI_MAG3] != ELF_MAG3))
        return ENOEXEC;

    assert(elf_head->ident[EI_CLASS]== ELF_CLASS32);
    assert(elf_head->type == ET_EXEC);
    assert(elf_head->machine == EM_386 || elf_head->machine == EM_486);
    assert(elf_head->program_head_number <= 3);
    assert(elf_head->program_head_entry_size == sizeof(program_head_t));
    return 0;
}

void clear_bss(program_head_t *p)
{
    vaddr_t bss_start = p->virtual_address + p->file_size;
    int bss_size = p->memory_size - p->file_size;
    memset((void *)bss_start, 0, bss_size);
}

int load_segment(inode_t *inode, program_head_t *p)
{
    vaddr_t start = p->virtual_address;
    vaddr_t end = p->virtual_address + p->memory_size;
    int r, w, x;
    segment_t *segment = NULL;

    r = p->flags & ELF_READ;
    w = p->flags & ELF_WRITE;
    x = p->flags & ELF_EXECUTE;
    if (r && x)
        segment = &this_pcb->code_segment;
    else if (r && w)
        segment = &this_pcb->data_segment;
    else
        assert(false);
    segment->start = start;
    segment->end = end;

    map_segment(this_pcb->pgtbl, segment, PTE_USER|PTE_WRITABLE|PTE_PRESENT);
    inode_read(inode, p->offset, (void *)segment->start, p->file_size);
    clear_bss(p);
    return 0;
}

#define PROGRAM_HEAD_MAX 6
int load_elf(inode_t *inode, elf_head_t *elf_head)
{
    int i;
    int error;
    program_head_t array[PROGRAM_HEAD_MAX];

    int offset = elf_head->program_head_offset;
    int number = elf_head->program_head_number;
    int size = number * sizeof(program_head_t);

    if (number > PROGRAM_HEAD_MAX)
        return ENOEXEC;
    if ((error = inode_read(inode, offset, array, size)) < 0)
        return error;

    for (i = 0; i < number; i++) {
        program_head_t *p = array + i;
        if (p->type == PT_LOAD)
            load_segment(inode, p);
    }
    return 0;
}

int sys_execve(char *path, char *argv[], char *envv[])
{
    volatile regs_t *regs = (regs_t *)&path; 
    int error = 0;
    elf_head_t elf_head;
    inode_t *inode = NULL;
    ustack_t ustack;

    regs = (regs_t *)&path;
    if (error = namei_open(path, &inode))
        return error;
    if ((error = inode_read(inode, 0, &elf_head, sizeof(elf_head_t))) < 0)
        goto bad;
    if (error = check_elf(inode, &elf_head))
        goto bad;

    ustack_init(&ustack);
    if (error = ustack_build(&ustack, argv, envv))
        goto bad;

    strcpy(this_pcb->name, argv[0]);
    flush_vm();
    if (error = load_elf(inode, &elf_head))
        goto bad;

    regs->eip = elf_head.entry;
    regs->esp = ustack_map(&ustack);

bad:inode_free(inode);
    return error;
}
