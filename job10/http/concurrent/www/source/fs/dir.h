#ifndef _FS_DIR_H
#define _FS_DIR_H

#include "inode.h"
#include "kernel/limits.h"

typedef struct dir_entry {
    char name[NAME_MAX];
    int ino;
} dir_entry_t;

extern void dir_entry_set(dir_entry_t *dir_entry, char *name, int ino);
extern int dir_add_entry(inode_t *this, char *name, int ino);
extern int dir_remove_entry(inode_t *this, char *name);

struct buffer;
typedef struct {
    inode_t *inode;
    int offset;
    struct buffer *buffer;
} dir_walker_t;

extern int dir_make_regular(inode_t *this, char *name, inode_t **result);
extern int dir_make_dir(inode_t *this, char *name, inode_t **result);
extern inode_t *dir_lookup_inode(inode_t *this, char *name);
extern bool dir_lookup_ino(inode_t *this, int ino, dir_entry_t *result);

enum {
    NAMEI_OPEN,
    NAMEI_MAKE,
};

extern int namei(int flag, char *path, inode_t **result, char *base_name);
extern int namei_open(char *path, inode_t **result);
extern int namei_make(char *path, inode_t **result, char *base_name);

#endif
