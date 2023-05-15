#ifndef _FS_BUFFER_H
#define _FS_BUFFER_H

#include <utils/list.h>
#include <kernel/sync.h>
#include <dev/hd.h>

#define BUFFER_COUNT 256
#define BLOCK_SIZE 512

enum {
    BUFFER_INVALID,
    BUFFER_CLEAN,
    BUFFER_DIRTY,
};

typedef struct buffer {
    chain_t hash_chain;
    chain_t lru_chain;
    int ref_count;
    int state;
    int locked;
    cond_t cond;
    int device;
    int block;
    void *data;
} buffer_t;

extern void buffer_table_init();
extern void buffer_hold(buffer_t *this);
extern void buffer_free(buffer_t *this);
extern void buffer_touch(buffer_t *this);

extern buffer_t *get_buffer(int device, int block);
extern buffer_t *read_buffer(int device, int block);

extern void buffer_start_io(buffer_t *this, int command);
extern void buffer_wait_io(buffer_t *this);
extern void buffer_sync_io(buffer_t *this, int command);
extern void buffer_end_io(buffer_t *this, int state);

#endif
