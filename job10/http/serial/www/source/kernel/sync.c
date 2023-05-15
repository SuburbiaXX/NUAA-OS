#include <utils/root.h>
#include <cpu/cpu.h>
#include <cpu/pmode.h>
#include "pcb.h"
#include "sync.h"

list_t running_pcb_list = {&running_pcb_list, &running_pcb_list};
list_t zombie_pcb_list = {&zombie_pcb_list, &zombie_pcb_list};
pcb_t *this_pcb; 

/* A round-robbin scheduler */
void yield_cpu(list_t *other_pcb_list, int state)
{
    pcb_t *that_pcb;

    chain_remove(this_pcb);
    list_push_back(other_pcb_list, this_pcb);
    this_pcb->state = state;

    that_pcb = list_front(&running_pcb_list);
    assert(that_pcb != NULL);
    if (that_pcb != this_pcb)
        switch_to(that_pcb);
}

void schedule()
{
    yield_cpu(&running_pcb_list, PCB_RUNNING);
}

void turn_into_zombie()
{
    yield_cpu(&zombie_pcb_list, PCB_ZOMBIE);
}

int sys_nice(int increment)
{
    schedule();
    return 0;
}

void cond_init(cond_t *this)
{
    list_init(this);
}

void cond_destroy(cond_t *this)
{
}    

void cond_wait(cond_t *this)
{
    yield_cpu(this, PCB_WAIT);
    if (has_signal())
        ;
}

void cond_signal(cond_t *this)
{
    pcb_t *pcb;
    
    pcb = list_pop_front(this);
    if (pcb) {
        pcb->state = PCB_RUNNING;
        list_push_back(&running_pcb_list, pcb);
    }
}

void cond_broadcast(cond_t *this)
{
    pcb_t *pcb;

    while (pcb = list_pop_front(this)) {
        pcb->state = PCB_RUNNING;
        list_push_back(&running_pcb_list, pcb);
    } 
}
