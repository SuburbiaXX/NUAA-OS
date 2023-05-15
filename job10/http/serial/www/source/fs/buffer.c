#include <utils/root.h>
#include <mm/memory.h>
#include <dev/number.h>
#include <cpu/irq.h>
#include "buffer.h"
#include "inode.h"

#define HASH_COUNT BUFFER_COUNT
#define HASH_MASK (HASH_COUNT - 1)

static list_t lru_list = {&lru_list, &lru_list};
static list_t hash_table[HASH_COUNT];

static list_t *hash_function(int device, int block)
{
    return hash_table + ((device + block) & HASH_MASK);
}

static void insert_into_hash_table(buffer_t *buffer)
{
    list_t *hash_list = hash_function(buffer->device, buffer->block);
    list_push_tail(hash_list, &buffer->hash_chain);
}

buffer_t *buffer_new()
{
    buffer_t *this;

    this = alloc_memory(sizeof(buffer_t));
    this->ref_count = 0;
    this->state = BUFFER_INVALID;
    this->locked = 0;
    cond_init(&this->cond);
    this->device = -1;
    this->block = -1;
    this->data = alloc_memory(BLOCK_SIZE);

    chain_init(&this->hash_chain);
    chain_init(&this->lru_chain);
    return this;
}

void buffer_table_init()
{
    int i;
    buffer_t *buffer;

    puts("init buffer cache");
    for (i = 0; i < HASH_COUNT; i++)
        list_init(&hash_table[i]);

    for (i = 0; i < HASH_COUNT; i++) {
        buffer = buffer_new();
        insert_into_hash_table(buffer);
        list_push_tail(&lru_list, &buffer->lru_chain);
    }
}

void buffer_hold(buffer_t *this)
{
    this->ref_count += 1;
}

void buffer_free(buffer_t *this)
{
    assert(this->ref_count > 0);
    this->ref_count -= 1;
}

buffer_t *get_buffer_from_hash(int device, int block)
{
    chain_t *chain;
    list_t *hash_list;
    buffer_t *buffer;
    
    hash_list = hash_function(device, block);
    list_each (hash_list, chain) {
        buffer = container_of(buffer_t, hash_chain, chain);
        if ((buffer->device == device) && (buffer->block == block))
            return buffer;
    }
    return NULL;
}

void buffer_mark_mru(buffer_t *this)
{
    chain_unlink(&this->lru_chain);
    list_push_tail(&lru_list, &this->lru_chain);
}

void buffer_touch(buffer_t *this)
{
    assert(this->state != BUFFER_INVALID);
    this->state = BUFFER_DIRTY;
}

buffer_t *get_buffer_from_lru()
{
    chain_t *chain;
    buffer_t *buffer;
   
    list_each (&lru_list, chain) {
        buffer = container_of(buffer_t, lru_chain, chain);

        if (buffer->state == BUFFER_DIRTY) {
            buffer_start_io(buffer, IO_WRITE);
            continue;
        }

        if (buffer->ref_count != 0)
            continue;

        return buffer;
    }
    return NULL;
}

buffer_t *get_buffer(int device, int block)
{
    buffer_t *buffer;

    buffer = get_buffer_from_hash(device, block);
    if (buffer) {
        buffer_mark_mru(buffer);
        buffer_hold(buffer);
        return buffer;
    }

    // Replace the buffer
    buffer = get_buffer_from_lru(device, block);
    assert(buffer);

    buffer->state = BUFFER_INVALID;
    buffer->device = device;
    buffer->block = block;
    chain_unlink(&buffer->hash_chain);
    chain_unlink(&buffer->lru_chain);

    insert_into_hash_table(buffer);
    list_push_tail(&lru_list, &buffer->lru_chain);

    buffer_hold(buffer);
    return buffer;
}

buffer_t *read_buffer(int device, int block)
{
    buffer_t *buffer;

    buffer = get_buffer(device, block);
    if (buffer->state != BUFFER_INVALID)
        return buffer;

    buffer_sync_io(buffer, IO_READ);
    if (buffer->state != BUFFER_INVALID)
        return buffer;

    buffer_free(buffer);
    return NULL;
}

void buffer_start_io(buffer_t *this, int command)
{
    buffer_hold(this);
    while (this->locked)
        cond_wait(&this->cond);
    this->locked = 1;
    hd_request(this, command);
}

void buffer_wait_io(buffer_t *this)
{
    assert(this->ref_count);
    while (this->locked)
        cond_wait(&this->cond);
}

void buffer_sync_io(buffer_t *this, int command)
{
    buffer_start_io(this, command);
    buffer_wait_io(this);
}

void buffer_end_io(buffer_t *this, int state)
{
    this->state = state;
    this->locked = 0;
    cond_broadcast(&this->cond);
    buffer_free(this);
}

void sync_all_buffer()
{
    chain_t *chain;
    buffer_t *buffer;

    list_each (&lru_list, chain) {
        buffer = container_of(buffer_t, lru_chain, chain);
        if (buffer->state == BUFFER_DIRTY) {
            buffer_sync_io(buffer, IO_WRITE);
        }
    }
}

int sys_sync()
{
    sync_all_inode();
    sync_all_buffer();
    return 0;
}
