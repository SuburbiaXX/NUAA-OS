#include <utils/root.h>
#include <kernel/file.h>

void memory_file_free(file_t *this)
{
}

int memory_file_read(file_t *this, void *buffer, int size)
{
    return 0;
}

int memory_file_write(file_t *this, void *buffer, int size)
{
    return 0;
}

int memory_file_lseek(file_t *this, off_t offset, int whence)
{
    return 0;
}

int memory_file_ioctl(file_t *this, int command, ulong arg)
{
    return 0;
}

file_class_t memory_file_class = {
    .free = memory_file_free,
    .read = memory_file_read,
    .write = memory_file_write,
    .lseek = memory_file_lseek,
    .ioctl = memory_file_ioctl
};
