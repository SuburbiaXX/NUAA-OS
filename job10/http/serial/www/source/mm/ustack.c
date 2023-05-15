#include <utils/root.h>
#include <kernel/errno.h>
#include <kernel/pcb.h>
#include "page.h"
#include "vm.h"
#include "ustack.h"

void ustack_init(ustack_t *this)
{
    this->page = alloc_page();
    this->sp = this->page + PAGE_SIZE;
    this->base = USER_END - PAGE_SIZE;
    this->argc = 0;
    this->argp = NULL;
    this->envc = 0;
    this->envp = NULL;
}

vaddr_t ustack_to_vaddr(ustack_t *this, void *p)
{
    return this->base + (p - this->page);
}

int ustack_push_string(ustack_t *this, char *string)
{
    int len = strlen(string);
    /* The trailing zero is also pushed onto stack */
    this->sp -= len + 1;
    if (this->sp < this->page)
        return EFAULT;
    strcpy(this->sp, string);
    return 0;
}

int ustack_push_vector(ustack_t *this, char *vector[])
{
    int error;
    int i;
    int count = 0;

    if ((error = verify_vector(vector)) < 0)
        return error;
 
    /* push in reverse order */
    for (i = error - 1; i >= 0; i--) {
        char *item = vector[i]; 
        if (error = verify_string(item))
            return error;
        if (error = ustack_push_string(this, item))
            return error;
        count++;
    }
    return count;
}

/**
 * sp:	 argc
 *	 argp
 *	 envp
 *
 * argp: argv[0]
 *       ...
 *       NULL 
 *
 * args: argv[0][0] argv[0][1] ... \0
 *       ...
 *       ...
 *
 * envp: envv[0]
 *       ...
 *       NULL 
 *
 * envs: envv[0][0] envv[0][1] ... \0 
 *       ...
 *       ...
 */
int ustack_push_argv(ustack_t *this, char *argv[])
{
    int error;
    int argc;
    char *args;
    long *lp;
    int i;

    error = ustack_push_vector(this, argv);
    if (error < 0)
        return error;
    argc = error;
    args = this->sp;

    lp = (long *)round_down((long)this->sp, sizeof(void *));
    lp -= argc + 1;
    this->sp = lp;
    this->argp = lp;
    if (this->sp < this->page)
        return EFAULT;

    for (i = 0; i < argc; i++) {
        *lp++ = ustack_to_vaddr(this, args);
        args += strlen(args) + 1;
    }
    *lp++ = 0;

    this->argc = argc;
    return 0;
}

int ustack_push_envv(ustack_t *this, char *envv[])
{
    int error;
    int envc;
    char *envs;
    long *lp;
    int i;

    error = ustack_push_vector(this, envv);
    if (error < 0)
        return error;
    envc = error;
    envs = this->sp;

    lp = (long *)round_down((long)this->sp, sizeof(void *));
    lp -= envc + 1;
    this->sp = lp;
    this->envp = lp;
    if (this->sp < this->page)
        return EFAULT;

    for (i = 0; i < envc; i++) {
        *lp++ = ustack_to_vaddr(this, envs);
        envs += strlen(envs) + 1;
    }
    *lp++ = 0;

    this->envc = envc;
    return 0;
}

int ustack_build(ustack_t *this, char *argv[], char *envv[])
{
    int error;
    long *lp;

    if (error = ustack_push_envv(this, envv))
        return error;
    if (error = ustack_push_argv(this, argv))
        return error;

    lp = this->sp; 
    lp -= 3;
    this->sp = lp;
    if (this->sp < this->page)
        return EFAULT;
    *lp++ = this->argc;
    *lp++ = ustack_to_vaddr(this, this->argp);
    *lp++ = ustack_to_vaddr(this, this->envp); 
    return 0;
}

vaddr_t ustack_map(ustack_t *this)
{
    pte_t *pte;
    paddr_t paddr;
   
    this_pcb->stack_segment.start = this->base;
    this_pcb->stack_segment.end = USER_END;  

    pte = pgtbl_create_pte(this_pcb->pgtbl, this->base);
    paddr = to_paddr(this->page);
    pte_make(pte, paddr, PTE_USER|PTE_WRITABLE|PTE_PRESENT);
    return ustack_to_vaddr(this, this->sp);
}
