#include <utils/root.h>
#include <utils/string.h>
#include <kernel/pcb.h>
#include <kernel/errno.h>
#include "buffer.h"
#include "fs.h"
#include "dir.h"

void dir_entry_set(dir_entry_t *dir_entry, char *name, int ino)
{
    strcpy(dir_entry->name, name);
    dir_entry->ino = ino;
}

void dir_walker_init(dir_walker_t *this, inode_t *inode)
{
    this->inode = inode;
    this->offset = 0;
    this->buffer = NULL;
}

dir_entry_t *dir_walker_next(dir_walker_t *this)
{
    inode_t *inode = this->inode;
    buffer_t *buffer = this->buffer;
    dir_entry_t *dir_entry;
    
    if (this->offset >= inode->size)
        return NULL;

    if (this->offset % BLOCK_SIZE == 0) {
        int block;
        if (buffer)
            buffer_free(buffer);
        block = inode_map(inode, this->offset / BLOCK_SIZE);
        buffer = this->buffer = read_buffer(inode->device, block);
    }

    dir_entry = buffer->data + (this->offset % BLOCK_SIZE);
    this->offset += sizeof(dir_entry_t);
    return dir_entry;
}

void dir_walker_destroy(dir_walker_t *this)
{
    if (this->buffer) {
        buffer_free(this->buffer);
        this->buffer = NULL;
    }
}

bool dir_lookup_name(inode_t *this, char *name, dir_entry_t *result)
{
    bool found = 0;
    dir_entry_t *dir_entry;
    dir_walker_t dir_walker;

    dir_walker_init(&dir_walker, this);
    while (dir_entry = dir_walker_next(&dir_walker)) {
        if (dir_entry->ino == FREE_INO)
            continue;
        if (strcmp(dir_entry->name, name) == 0) {
            found = 1;
            *result = *dir_entry;
            break;
        }
    }
    dir_walker_destroy(&dir_walker);
    return found;
}

bool dir_lookup_ino(inode_t *this, int ino, dir_entry_t *result)
{
    bool found = 0;
    dir_entry_t *dir_entry;
    dir_walker_t dir_walker;

    dir_walker_init(&dir_walker, this);
    while (dir_entry = dir_walker_next(&dir_walker)) {
        if (dir_entry->ino == FREE_INO)
            continue;
        if (dir_entry->ino == ino) {
            found = 1;
            *result = *dir_entry; 
            break;
        }
    }
    dir_walker_destroy(&dir_walker);
    return found;
}

inode_t *dir_lookup_inode(inode_t *this, char *name)
{
    dir_entry_t dir_entry; 

    if (!dir_lookup_name(this, name, &dir_entry))
        return NULL;
    return read_inode(this->fs, dir_entry.ino);
}

int dir_remove_entry(inode_t *this, char *name)
{
    int ino = FREE_INO;
    dir_entry_t *dir_entry;
    dir_walker_t dir_walker;

    dir_walker_init(&dir_walker, this);
    while (dir_entry = dir_walker_next(&dir_walker)) {
        if (dir_entry->ino == FREE_INO)
            continue;
        if (strcmp(dir_entry->name, name) == 0) {
            ino = dir_entry->ino;
            memset(dir_entry, 0, sizeof(dir_entry_t));
            break;
        }
    }
    dir_walker_destroy(&dir_walker);
    return ino;
}

bool dir_reuse_entry(inode_t *this, char *name, int ino)
{
    bool found = 0;
    dir_walker_t dir_walker;
    dir_entry_t *dir_entry;

    dir_walker_init(&dir_walker, this);
    while (dir_entry = dir_walker_next(&dir_walker)) {
        if (dir_entry->ino == FREE_INO) {
            found = 1;
            dir_entry_set(dir_entry, name, ino);
            break;
        }
    }
    dir_walker_destroy(&dir_walker);
    return found;
}

int dir_add_entry(inode_t *this, char *name, int ino)
{
    int written;
    dir_entry_t dir_entry;

    if (dir_lookup_name(this, name, &dir_entry))
        return EEXIST;

    inode_touch(this);
    if (dir_reuse_entry(this, name, ino))
        return 0;

    dir_entry_set(&dir_entry, name, ino);
    written = inode_write(this, this->size, &dir_entry, sizeof(dir_entry_t));
    if (written < 0)
        return written;
    return 0;
}

int dir_make_common(inode_t *this, char *name, int mode, inode_t **result)
{
    int error;
    int ino;
    inode_t *leaf;

    *result = NULL;
    ino = fs_alloc_ino(this->fs);
    if (ino == FREE_INO)
        return ENOSPC;

    if (error = dir_add_entry(this, name, ino)) {
        fs_free_ino(this->fs, ino);
        return error;
    }

    leaf = get_inode(this->fs, ino);
    leaf->state = INODE_DIRTY;
    leaf->link_count = 0;
    leaf->mode = mode;
    leaf->uid = this_pcb->uid;
    leaf->gid = this_pcb->gid;
    leaf->real_device = 0;
    leaf->size = 0;

    *result = leaf;
    return 0;
}

int dir_make_regular(inode_t *this, char *name, inode_t **result)
{
    int mode = S_IFREG | 0666;
    int error;
    inode_t *leaf;

    error = dir_make_common(this, name, mode, &leaf);
    if (error < 0)
        return error;
    leaf->link_count = 1;
    *result = leaf;
    return 0;
}

int dir_make_dir(inode_t *this, char *name, inode_t **result)
{
    int mode = S_IFDIR | 0666;
    int error;
    inode_t *leaf;

    error = dir_make_common(this, name, mode, &leaf);
    if (error < 0)
        return error;

    dir_add_entry(leaf, ".", leaf->ino);
    dir_add_entry(leaf, "..", this->ino);
    this->link_count += 1;
    leaf->link_count = 2;
    *result = leaf;
    return 0;
}

int namei(int flag, char *path_read_only, inode_t **result, char *base_name)
{
    char path[PATH_MAX];
    int i;
    char *word_table[COMP_MAX];
    int word_count;
    char *word;

    int error;
    inode_t *parent;
    inode_t *child;

    strcpy(path, path_read_only);
    parent = (path[0] == '/') ? this_pcb->root_dir : this_pcb->work_dir;
    inode_hold(parent);
    child = NULL;

    word_count = split_string(path, "/", word_table);
    for (i = 0; i < word_count; i++) {
        if (!S_ISDIR(parent->mode))
            raise(ENOTDIR);

        word = word_table[i];
        if (i == word_count - 1 && flag == NAMEI_MAKE) {
            *result = parent;
            strncpy(base_name, word, NAME_MAX);
            base_name[NAME_MAX - 1] = 0;
            return 0;
        }

        child = dir_lookup_inode(parent, word);
        if (!child)
            raise(ENOENT);
        inode_free(parent);
        parent = child;
    }

    *result = parent;
    return 0;

bad:inode_free(parent);
    *result = NULL;
    return error;
}

int namei_open(char *path, inode_t **result)
{
    return namei(NAMEI_OPEN, path, result, NULL);
}

int namei_make(char *path, inode_t **result, char *base_name)
{
    return namei(NAMEI_MAKE, path, result, base_name);
}
