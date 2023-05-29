#include <utils/root.h>
#include <mm/vm.h>
#include <mm/memory.h>
#include "pcb.h"
#include "fcntl.h"
#include "file.h"
#include "errno.h"

file_t *file_new(int type, int flags, void *pointer)
{
    file_t *this = alloc_memory(sizeof(file_t));
    if (!this)
        return NULL;

    this->class = NULL;
    this->type = type;
    this->ref_count = 0;
    this->flags = flags;
    this->offset = 0;
    this->pointer = pointer;
    return this;
}

void file_delete(file_t *this)
{
    assert(this->ref_count == 0);
    free_memory(this);
}

void file_hold(file_t *this) 
{ 
    assert(this->ref_count >= 0); 
    this->ref_count++; 
}

void file_free(file_t *this) 
{
    assert(this && this->ref_count > 0); 
    this->ref_count -= 1;
    if (this->ref_count)
        return;
    this->class->free(this);
    file_delete(this);
}

int file_permit_read(file_t *this)
{
    return (this->flags & O_ACCMODE) != O_WRONLY;
}

int file_permit_write(file_t *this)
{
    return (this->flags & O_ACCMODE) != O_RDONLY;
}

int verify_fd(int fd)
{
    if (fd < 0 || fd >= OPEN_MAX)
        return EBADF;
    return 0;
}

file_t *get_file(int fd)
{
    if (fd < 0 || fd >= OPEN_MAX)
        return NULL;
    return this_pcb->file_table[fd];
}

int close_file(int fd)
{
    file_t *file;
    
    file = get_file(fd);
    if (!file)
        return EBADF;
    file_free(file);
    this_pcb->file_table[fd] = NULL;
    return 0;
}

void close_all_file()
{
    int i;
    
    for (i = 0; i < OPEN_MAX; i++) {
        if (this_pcb->file_table[i])
            close_file(i);
    }
}

int alloc_fd(int *fd)
{
    int i;

    for (i = 0; i < OPEN_MAX ; i++) {
        if (!this_pcb->file_table[i]) {
            *fd = i;
            return 0;
        }
    }
    return EMFILE;
}

int alloc_fd_pair(int fd[2])
{
    int i, count = 0;

    for (i = 0; i < OPEN_MAX; i++) {
        if (!this_pcb->file_table[i]) {
            fd[count++] = i;
            if (count == 2)
                return 0;
        }
    }
    return EMFILE;
}

void install_file(int fd, file_t *file)
{
    assert(fd >= 0 && fd < OPEN_MAX);
    file_hold(file);
    this_pcb->file_table[fd] = file;
}

int sys_dup(int old_fd)
{
    int new_fd = 0;
    file_t *file;
   
    file = get_file(old_fd);
    if (!file)
        return EBADF;

    for (; new_fd < OPEN_MAX; new_fd++) {
        if (this_pcb->file_table[new_fd])
            continue;

        this_pcb->file_table[new_fd] = file;
        file_hold(file);
        return new_fd;
    }
    return EMFILE;
}

int sys_dup2(int old_fd, int new_fd)
{
    file_t *file;

    file = get_file(old_fd);
    if (!file)
        return EBADF;
    if (old_fd == new_fd)
        return new_fd;

    close_file(new_fd);
    this_pcb->file_table[new_fd] = file;
    file_hold(file);
    return new_fd;
}

int sys_read(int fd, void *buffer, size_t size)
{
    int error;
    file_t *file;

    file = get_file(fd);
    if (!file)
        return EBADF; 
    if (!file_permit_read(file))
        raise(EBADF);
    if (error = verify_write(buffer, size))
        goto bad;
    error = file->class->read(file, buffer, size);
    return error;

bad:file_free(file);
    return error;
}

int sys_write(int fd, void *buffer, size_t size)
{
    int error;
    file_t *file;

    file = get_file(fd);
    if (!file)
        return EBADF;
    if (!file_permit_write(file))
        raise(EBADF);
    if (error = verify_read(buffer, size))
        goto bad;
    error = file->class->write(file, buffer, size);
    return error;

bad:file_free(file);
    return error;
}

int sys_lseek(int fd, off_t offset, int whence)
{
    int error;
    file_t *file;
    
    file = get_file(fd);
    if (!file)
        return EBADF;
    error = file->class->lseek(file, offset, whence);
    return error;
}

int sys_ioctl(int fd, int command, ulong param)
{
    int error;
    file_t *file;

    file = get_file(fd);
    if (!file)
        return EBADF;
    error = file->class->ioctl(file, command, param);
    return error;
}

int sys_close(int fd)
{
    return close_file(fd);
}
