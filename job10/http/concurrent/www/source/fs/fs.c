#include <utils/root.h>
#include <utils/bitmap.h>
#include <mm/memory.h>
#include <dev/number.h>
#include <kernel/pcb.h>
#include <cpu/irq.h>
#include "fs.h"

int root_device;
inode_t *root_dir;
fs_t *root_fs;

int fs_alloc_ino(fs_t *this)
{
    int i;

    for (i = 0; i < this->inode_bitmap.block_count; i++) {
        buffer_t *buffer = this->inode_bitmap_buffer[i];
        int ino = alloc_bit(buffer->data, BLOCK_SIZE);
        if (ino >= 0) {
            buffer_touch(buffer);
            return ino;
        }
    }
    return FREE_INO;
}

int fs_free_ino(fs_t *this, int ino)
{
    int div = ino / BITS_PER_BLOCK;
    int mod = ino % BITS_PER_BLOCK;
    buffer_t *buffer = this->inode_bitmap_buffer[div];
    free_bit(buffer->data, mod);
    buffer_touch(buffer);
    return 0;
}

int fs_alloc_block(fs_t *this)
{
    int i;

    for (i = 0; i < this->data_bitmap.block_count; i++) {
        buffer_t *buffer = this->data_bitmap_buffer[i];
        int block = alloc_bit(buffer->data, BLOCK_SIZE);
        if (block >= 0) {
            buffer_touch(buffer);
            return this->data_table.start_block + block;
        }
    }
    return FREE_BLOCK;
}

int fs_free_block(fs_t *this, int block)
{
    int bit = block - this->data_table.start_block;
    int div = bit / BITS_PER_BLOCK;
    int mod = bit % BITS_PER_BLOCK;
    buffer_t *buffer;

    assert(bit >= 0);
    buffer = this->data_bitmap_buffer[div];
    free_bit(buffer->data, mod);
    buffer_touch(buffer);
    return 0;
}

void verify_consts()
{
    assert(BLOCK_SIZE % sizeof(dir_entry_t) == 0);
}

int fs_load_inode(fs_t *this, inode_t *inode)
{
    buffer_t *buffer;
    disk_inode_t *disk_inode;
    int offset = inode->ino * sizeof(disk_inode_t);
    int div = offset / BLOCK_SIZE;
    int mod = offset % BLOCK_SIZE;

    buffer = read_buffer(this->device, this->inode_table.start_block + div); 
    disk_inode = buffer->data + mod;
    inode->state = INODE_CLEAN;
    inode->link_count = disk_inode->link_count;
    inode->mode = disk_inode->mode;
    inode->uid = disk_inode->uid;
    inode->gid = disk_inode->gid;
    inode->real_device = disk_inode->real_device;
    inode->size = disk_inode->size;
    memcpy(inode->address, disk_inode->address, sizeof(inode->address));

    buffer_free(buffer);
    return 0;
}

int fs_store_inode(fs_t *this, inode_t *inode)
{
    buffer_t *buffer;
    disk_inode_t *disk_inode;
    int offset = inode->ino * sizeof(disk_inode_t);
    int div = offset / BLOCK_SIZE;
    int mod = offset % BLOCK_SIZE;

    buffer = read_buffer(this->device, this->inode_table.start_block + div); 
    disk_inode = buffer->data + mod;
    disk_inode->link_count = inode->link_count;
    disk_inode->mode = inode->mode;
    disk_inode->uid = inode->uid;
    disk_inode->gid = inode->gid;
    disk_inode->real_device = inode->real_device;
    disk_inode->size = inode->size;
    memcpy(disk_inode->address, inode->address, sizeof(inode->address));

    inode->state = INODE_CLEAN;
    buffer->state = BUFFER_DIRTY;
    buffer_free(buffer);
    return 0;
}

void fs_dump(fs_t *this)
{
    assert(this->magic == FS_MAGIC);
    printf("magic = %x\n", this->magic);
    printf("inode_count = %d\n", this->inode_count);
    printf("data_count = %d\n", this->data_count);
}

buffer_t **fs_load_bitmap(fs_t *this, disk_region_t *region)
{
    buffer_t **array;
    int i;

    array = alloc_memory(region->block_count * sizeof(buffer_t *));
    for (i = 0; i < region->block_count; i++) {
        int block = region->start_block + i;
        array[i] = read_buffer(this->device, block);
    }
    return array;
}

int fs_mount(fs_t *this, int device)
{
    buffer_t *head;
    disk_fs_t *disk_fs;

    head = read_buffer(device, 0);
    disk_fs = head->data;

    chain_init(&this->chain);
    this->device = device;

    this->magic = disk_fs->magic;
    this->inode_count = disk_fs->inode_count;
    this->data_count = disk_fs->data_count;
    this->inode_bitmap = disk_fs->inode_bitmap;
    this->inode_table = disk_fs->inode_table;
    this->data_bitmap = disk_fs->data_bitmap;
    this->data_table = disk_fs->data_table;

    this->inode_bitmap_buffer = fs_load_bitmap(this, &this->inode_bitmap); 
    this->data_bitmap_buffer = fs_load_bitmap(this, &this->data_bitmap); 

    printf("mount fs ok\n");
    buffer_free(head);
    return 0;
}

void fs_sync(fs_t *this)
{
    int i;

    for (i = 0; i < this->inode_bitmap.block_count; i++) {
        buffer_t *buffer = this->inode_bitmap_buffer[i];
        if (buffer->state == BUFFER_DIRTY)
            buffer_sync_io(buffer, IO_WRITE);
    }

    for (i = 0; i < this->data_bitmap.block_count; i++) {
        buffer_t *buffer = this->data_bitmap_buffer[i];
        if (buffer->state == BUFFER_DIRTY)
            buffer_sync_io(buffer, IO_WRITE);
    }
}

void print_motd()
{
    inode_t *inode;

    namei_open("/etc/hostname", &inode);
}

int sys_setup()
{
    enable_all_irq();
    root_device = device_make(MAJOR_HARD_DISK, 0);
    root_fs = alloc_memory(sizeof(fs_t));
    fs_mount(root_fs, root_device);
    root_dir = read_inode(root_fs, ROOT_INO); 
    this_pcb->root_dir = this_pcb->work_dir = root_dir;
    root_dir->ref_count += 2;
    assert(root_dir->ref_count == 3);
    return 0;
}
