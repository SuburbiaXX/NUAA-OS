#include <utils/root.h>
#include <mm/memory.h>
#include "errno.h"
#include "file.h"
#include "pipe.h"

pipe_t *pipe_new()
{
    pipe_t *this;
    void *page;

    this = alloc_memory(sizeof(pipe_t));
    page = alloc_page();
    cond_init(&this->cond);
    fifo_init(&this->fifo, page, PAGE_SIZE);
    this->reader_count = 1;
    this->writer_count = 1;
    return this;
}

int pipe_read(pipe_t *this, void *buffer, int size)
{
    fifo_t *fifo = &this->fifo;
    int readed;

    while (fifo_empty(fifo)) {
        if (!this->writer_count)
            return 0;
        cond_wait(&this->cond);
    }

    readed = fifo_read(fifo, buffer, size);
    cond_broadcast(&this->cond);
    return readed;
}

int pipe_write(pipe_t *this, void *buffer, int size)
{
    void *p = buffer;
    void *q = buffer + size;
    fifo_t *fifo = &this->fifo;

    while (p < q) {
        if (!this->reader_count)
            return EPIPE;

        if (fifo_full(fifo)) {
            cond_wait(&this->cond);
            continue;
        }

        p += fifo_write(fifo, p, q - p);
        cond_broadcast(&this->cond);
    }
    return p - buffer;
}

void pipe_delete(pipe_t *this)
{
    cond_destroy(&this->cond);
    free_page(this->fifo.buffer);
}

void pipe_file_free(file_t *this)
{
    pipe_t *pipe = this->pipe;

    if (file_permit_read(this))
        pipe->reader_count -= 1;
    if (file_permit_write(this))
        pipe->writer_count -= 1;
    if (pipe->reader_count == 0 && pipe->writer_count == 0)
        pipe_delete(pipe);
}

int pipe_file_read(file_t *this, void *buffer, int size)
{
    return pipe_read(this->pipe, buffer, size);
}

int pipe_file_write(file_t *this, void *buffer, int size)
{
    return pipe_write(this->pipe, buffer, size);
}

int pipe_file_lseek(file_t *this, off_t offset, int whence)
{
    return ESPIPE;
}

int pipe_file_ioctl(file_t *this, int request, ulong arg)
{
    return ENOTTY;
}

file_class_t pipe_file_class = {
    .free = pipe_file_free,
    .read = pipe_file_read,
    .write = pipe_file_write,
    .lseek = pipe_file_lseek,
    .ioctl = pipe_file_ioctl,
};

int sys_pipe(int fd[2])
{
    int error;
    pipe_t *pipe;
    file_t *reader;
    file_t *writer;

    if (error = alloc_fd_pair(fd))
        return error;

    pipe = pipe_new();
    reader = file_new(FILE_PIPE, O_RDONLY, pipe);
    writer = file_new(FILE_PIPE, O_WRONLY, pipe);
    reader->class = &pipe_file_class;
    writer->class = &pipe_file_class;

    install_file(fd[0], reader);
    install_file(fd[1], writer);
    return 0;
}
