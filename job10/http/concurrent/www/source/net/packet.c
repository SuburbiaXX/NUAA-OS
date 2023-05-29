#include <utils/root.h>
#include <mm/memory.h>
#include "packet.h"

packet_t *packet_new(int head_size, int data_size)
{
    int total_size = head_size + data_size;
    void *room = alloc_memory(total_size);
    if (room == NULL)
        return NULL;

    packet_t *this = alloc_memory(sizeof(packet_t));
    if (this == NULL) {
        free_memory(room);
        return NULL;
    }

    chain_init(&this->chain);
    this->room = room;
    this->bound = room + total_size;
    this->data = room + data_size; 
    this->data_size = data_size;
    return this;
}

void packet_delete(packet_t *this)
{
    free_memory(this->room);
    free_memory(this);
}

void packet_push(packet_t *this, int head_size)
{
    this->data -= head_size;
    this->data_size += head_size;
    assert(this->room <= this->data);
}

void packet_pop(packet_t *this, int head_size)
{
    this->data += head_size;
    this->data_size -= head_size;
    assert(this->data < this->bound);
}
