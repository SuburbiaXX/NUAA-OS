#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include "buffer.h"
#include "inode.h"
#include "dir.h"

typedef struct {
    int start_block;
    int block_count;
} disk_region_t;

#define FS_MAGIC 0x20130530
typedef struct disk_fs {
    int magic;
    int inode_count;
    int data_count;
    disk_region_t inode_bitmap;
    disk_region_t inode_table;
    disk_region_t data_bitmap;
    disk_region_t data_table;
} disk_fs_t;

#define FREE_INO 0
#define FREE_BLOCK 0
#define ROOT_INO 1
typedef struct fs {
    chain_t chain;
    int device;
    int magic;
    int inode_count;
    int data_count;
    disk_region_t inode_bitmap;
    disk_region_t inode_table;
    disk_region_t data_bitmap;
    disk_region_t data_table;
    buffer_t **inode_bitmap_buffer;
    buffer_t **data_bitmap_buffer;
} fs_t;

#define BITS_PER_BLOCK (BLOCK_SIZE * 8)
#define INODES_PER_BLOCK (BLOCK_SIZE / sizeof(disk_inode_t))

extern int fs_alloc_ino(fs_t *this);
extern int fs_free_ino(fs_t *this, int ino);
extern int fs_alloc_block(fs_t *this);
extern int fs_free_block(fs_t *this, int block);
extern int fs_load_inode(fs_t *this, inode_t *inode);
extern int fs_store_inode(fs_t *this, inode_t *inode);

extern int root_device;
extern inode_t *root_dir;
extern fs_t *root_fs;

#endif
