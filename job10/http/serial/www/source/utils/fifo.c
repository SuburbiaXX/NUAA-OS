#include <utils/root.h>
#include "fifo.h"

void fifo_init(fifo_t *this, char *buffer, int capacity)
{
    this->buffer = buffer;
    this->bound = buffer + capacity;
    this->capacity = capacity;
    fifo_clear(this);
}

void fifo_clear(fifo_t *this)
{
    this->size = 0;
    this->in = this->out = this->buffer;
}

int fifo_empty(fifo_t *this) 
{ 
    return this->size == 0;
}

int fifo_full(fifo_t *this) 
{ 
    return this->size == this->capacity;
}

char *fifo_inc(fifo_t *this, char *p)
{
    p += 1;
    if (p == this->bound)
        p = this->buffer;
    return p;
}

char *fifo_dec(fifo_t *this, char *p)
{
    if (p == this->buffer)
        p = this->bound;
    p -= 1;
    return p;
}

int fifo_get_char(fifo_t *this)
{
    assert(!fifo_empty(this));
    char c = *this->out;

    this->out = fifo_inc(this, this->out);
    this->size--;
    return c;
}

void fifo_unget_char(fifo_t *this, char c)
{
    assert(!fifo_full(this));
    this->out = fifo_dec(this, this->out);
    this->size++;

    *this->out = c;
}

void fifo_put_char(fifo_t *this, char c)
{
    assert(!fifo_full(this));
    *this->in = c;

    this->in = fifo_inc(this, this->in);
    this->size++;
}

int fifo_unput_char(fifo_t *this)
{
    assert(!fifo_empty(this));
    this->in = fifo_dec(this, this->in);
    this->size--;

    char c = *this->in; 
    return c;
}

int fifo_read(fifo_t *this, void *buffer, int left)
{
    int readed = 0;
    left = min(left, this->size);

    while (left) {
        int chunk = min(left, this->bound - this->out);
        memcpy(buffer, this->out, chunk);

        this->size -= chunk;
        this->out += chunk;
        if (this->out == this->bound)
            this->out = this->buffer;

        readed += chunk;
        buffer += chunk;
        left -= chunk;
    }
    return readed;
}

int fifo_write(fifo_t *this, void *buffer, int left)
{
    int written = 0;

    while (left) {
        int chunk = min(left, this->bound - this->in);
        memcpy(this->in, buffer, chunk);

        this->size += chunk;
        this->in += chunk;
        if (this->in == this->bound)
            this->in = this->buffer;
        
        written += chunk;
        buffer += chunk;
        left -= chunk;
    }
    return written;
}

void fifo_dump(fifo_t *this)
{
    printf("fifo\n");
    printf("  size = %d\n", this->size);
    printf("  in   = %p\n", this->in);
    printf("  out  = %p\n", this->out);
    printf("  buf  = %p\n", this->buffer);
    printf("  bnd  = %p\n", this->bound);
}
