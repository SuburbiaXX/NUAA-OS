#include <utils/root.h>
#include <mm/memory.h>
#include <kernel/errno.h>
#include <kernel/file.h>
#include "fs.h"
#include "inode.h"
#include "buffer.h"

int inode_translate(inode_t *this, int *entry)
{
    int address = *entry;
    if (address == FREE_BLOCK) {
        address = fs_alloc_block(this->fs);
        *entry = address;
    }
    return address;
}

int inode_map(inode_t *this, int block)
{
    int address;
    buffer_t *buffer;
    int *entry;

    if (block < DIRECT_BLOCK_COUNT) {
        address = inode_translate(this, this->address + block);
        return address;
    }

    block -= DIRECT_BLOCK_COUNT;
    address = inode_translate(this, this->address + DIRECT_BLOCK_COUNT);
    buffer = read_buffer(this->device, address);

    entry = ((int *)buffer->data) + block;
    address = inode_translate(this, entry);

    buffer_free(buffer);
    return address;
}

int inode_trunc(inode_t *this)
{
    int from = 0;
    int to = (this->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int block;
    
    for (block = from; block < to; block++) {
        int address = inode_map(this, block);
        fs_free_block(this->fs, address);
    }

    if (block > DIRECT_BLOCK_COUNT) {
        int address = this->address[DIRECT_BLOCK_COUNT];
        fs_free_block(this->fs, address);
    }

    this->size = 0;
    inode_touch(this);
    return 0;
}

int inode_grow(inode_t *this, int size)
{
    int from = (this->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int to = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int block;

    assert(this->size < size);
    for (block = from; block < to; block++)
        inode_map(this, block);
    this->size = size;
    inode_touch(this);
    return 0;
}

int inode_read(inode_t *this, off_t position, void *memory, int size)
{
    void *p = memory;
    void *bound;

    if (position >= this->size)
        return 0;
    if (position + size > this->size)
        size = this->size - position;

    bound = memory + size;
    while (p < bound) {
        buffer_t *buffer;
        int block = position / BLOCK_SIZE;
        int offset = position % BLOCK_SIZE; 
        int chunk = min(bound - p, BLOCK_SIZE - offset);
        block = inode_map(this, block);
        if (block == FREE_BLOCK)
            return EIO;
        buffer = read_buffer(this->device, block);
        if (!buffer)
            return EIO;
        memcpy(p, buffer->data + offset, chunk);

        position += chunk;
        p += chunk;
    }
    return p - memory;
}

int inode_write(inode_t *this, off_t position, void *memory, int size)
{
    void *p = memory;
    void *bound = memory+ size;

    inode_touch(this);
    if (position + size > this->size)
        inode_grow(this, position + size);
    while (p < bound) {
        buffer_t *buffer;
        int block = position / BLOCK_SIZE;
        int offset = position % BLOCK_SIZE; 
        int chunk = min(bound - p, BLOCK_SIZE - offset);

        block = inode_map(this, block);
        if (block == FREE_BLOCK)
            return EIO;
        if (chunk == BLOCK_SIZE)
            buffer = get_buffer(this->device, block);
        else
            buffer = read_buffer(this->device, block);
        if (!buffer)
            return EIO;

        memcpy(buffer->data + offset, p, chunk);
        buffer->state = BUFFER_DIRTY;

        position += chunk;
        p += chunk;
    }
    return p - memory;
}

void regular_file_free(file_t *this)
{
    inode_free(this->inode);
}

int regular_file_read(file_t *this, void *buffer, int size)
{
    int readed;

    readed = inode_read(this->inode, this->offset, buffer, size);
    if (readed < 0)
        return readed;
    this->offset += readed;
    return readed;
}

int regular_file_write(file_t *this, void *buffer, int size)
{
    int written;

    if (this->flags & O_APPEND)
        this->offset = this->inode->size;

    written = inode_write(this->inode, this->offset, buffer, size);
    if (written < 0)
        return written;
    this->offset += written;
    return written;
}

int regular_file_lseek(file_t *this, off_t offset, int whence)
{
    off_t off;

    switch (whence) {
        case SEEK_SET:
            off = offset;
            break;

        case SEEK_CUR:
            off = this->offset + offset;
            break;

        case SEEK_END:
            off = this->inode->size + offset;
            break;

        default:
            return EINVAL;
    }

    if (off < 0)
        return EINVAL;
    this->offset = off;
    return off;
}

int regular_file_ioctl(file_t *this, int request, ulong arg)
{
    return ENOTTY;
}

file_class_t regular_file_class = {
    .free = regular_file_free,
    .read = regular_file_read,
    .write = regular_file_write,
    .lseek = regular_file_lseek,
    .ioctl = regular_file_ioctl,
};
