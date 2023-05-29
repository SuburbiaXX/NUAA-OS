#ifndef _FS_INODE_H
#define _FS_INODE_H

#include <kernel/sync.h>
#include "stat.h"

struct fs;

/* inode state */
enum {
    INODE_INVALID,
    INODE_CLEAN,
    INODE_DIRTY,
};

#define DIRECT_BLOCK_COUNT 9
typedef struct disk_inode {
    int link_count;
    int mode;
    uid_t uid;
    gid_t gid;
    int real_device;
    int size;
    int address[DIRECT_BLOCK_COUNT + 1];
} disk_inode_t;

typedef struct inode {
    chain_t hash_chain;
    chain_t lru_chain;
    int state;
    int ref_count;
    int locked;
    cond_t cond;

    struct fs *fs;
    int device;
    int ino;

    int link_count;
    int mode;
    uid_t uid;
    gid_t gid;
    int real_device;
    int size;
    int address[DIRECT_BLOCK_COUNT + 1];
} inode_t;

#define INODE_COUNT 128

extern void inode_table_init();
extern inode_t *read_inode(struct fs *fs, int ino);
extern inode_t *get_inode(struct fs *fs, int ino);

extern void inode_hold(inode_t *this);
extern void inode_free(inode_t *this);
extern void inode_touch(inode_t *this);
extern void sync_all_inode();

extern int inode_map(inode_t *this, int block);
extern int inode_read(inode_t *this, off_t position, void *memory, int size);
extern int inode_write(inode_t *this, off_t position, void *memory, int size);
extern int inode_trunc(inode_t *this);

#endif
