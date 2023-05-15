#include <utils/root.h>
#include <mm/memory.h>
#include "inode.h"
#include "fs.h"
#include "buffer.h"

#define HASH_COUNT INODE_COUNT
#define HASH_MASK (HASH_COUNT - 1)

static list_t lru_list = {&lru_list, &lru_list};
static list_t hash_table[HASH_COUNT];

static list_t *hash_function(int device, int ino)
{
    return hash_table + ((device + ino) & HASH_MASK);
}

static void insert_into_hash_table(inode_t *inode)
{
    list_t *hash_list = hash_function(inode->device, inode->ino);
    list_push_tail(hash_list, &inode->hash_chain);
}

inode_t *inode_new()
{
    inode_t *this;

    this = alloc_memory(sizeof(inode_t));
    chain_init(&this->hash_chain);
    chain_init(&this->lru_chain);

    this->state = INODE_INVALID;
    this->ref_count = 0;
    this->locked = 0;
    cond_init(&this->cond);

    this->fs = NULL;
    this->device = -1;
    this->ino = -1;

    return this;
}

void inode_table_init()
{
    int i;
    inode_t *inode;

    puts("init inode cache");
    for (i = 0; i < HASH_COUNT; i++)
        list_init(&hash_table[i]);

    for (i = 0; i < INODE_COUNT; i++) {
        inode = inode_new();
        insert_into_hash_table(inode);
        list_push_tail(&lru_list, &inode->lru_chain);
    }
}

void inode_hold(inode_t *this)
{
    this->ref_count++;
}

void inode_free(inode_t *this)
{
    assert(this->ref_count > 0);
    this->ref_count -= 1;
    if (this->ref_count > 0)
        return;

    if (this->state == INODE_INVALID)
        return;
    if (this->link_count > 0)
        return;

    inode_trunc(this);
    fs_free_ino(this->fs, this->ino);
    this->state = INODE_INVALID;
    this->fs = NULL;
    this->device = -1;
    this->ino = -1;
}

void inode_touch(inode_t *this)
{
    assert(this->state != INODE_INVALID);
    this->state = INODE_DIRTY;
}

inode_t *get_inode_from_hash(fs_t *fs, int ino)
{
    chain_t *chain;
    list_t *hash_list;
    inode_t *inode;

    hash_list = hash_function(fs->device, ino);
    list_each (hash_list, chain) {
        inode = container_of(inode_t, hash_chain, chain);
        if ((inode->fs == fs) && (inode->ino == ino))
            return inode;
    }
    return NULL;
}

inode_t *get_inode_from_lru()
{
    chain_t *chain;
    inode_t *inode;
   
    list_each (&lru_list, chain) {
        inode = container_of(inode_t, lru_chain, chain);

        if (inode->state == INODE_DIRTY) {
            fs_store_inode(inode->fs, inode);
            continue;
        }

        if (inode->ref_count != 0)
            continue;

        return inode;
    }
    return NULL;
}

void inode_mark_mru(inode_t *this)
{
    chain_unlink(&this->lru_chain);
    list_push_tail(&lru_list, &this->lru_chain);
}

inode_t *get_inode(fs_t *fs, int ino)
{
    inode_t *inode;

    inode = get_inode_from_hash(fs, ino);
    if (inode) {
        inode_mark_mru(inode);
        inode_hold(inode);
        return inode;
    }

    // Replace the inode
    inode = get_inode_from_lru(fs, ino);
    assert(inode);

    inode->state = INODE_INVALID;
    inode->fs = fs;
    inode->device = fs->device;
    inode->ino = ino;
    chain_unlink(&inode->hash_chain);
    chain_unlink(&inode->lru_chain);

    insert_into_hash_table(inode);
    list_push_tail(&lru_list, &inode->lru_chain);

    inode_hold(inode);
    return inode;
}

inode_t *read_inode(fs_t *fs, int ino)
{
    inode_t *inode;

    inode = get_inode(fs, ino);
    if (inode->state != INODE_INVALID)
        return inode;
    fs_load_inode(fs, inode);
    return inode;
}

void sync_all_inode()
{
    chain_t *chain;
    inode_t *inode;

    list_each (&lru_list, chain) {
        inode = container_of(inode_t, lru_chain, chain);
        if (inode->state == INODE_DIRTY)
            fs_store_inode(inode->fs, inode);;
    }
}
