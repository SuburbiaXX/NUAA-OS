#ifndef _UTILS_FIFO_H
#define _UTILS_FIFO_H

// . A B C .
//   ^     ^
//   out   in
typedef struct {
    char *buffer;
    char *bound;
    int capacity;
    int size;
    char *in;
    char *out;
} fifo_t;

extern void fifo_init(fifo_t *this, char *buffer, int capacity);
extern void fifo_clear(fifo_t *this);

extern int fifo_empty(fifo_t *this);
extern int fifo_full(fifo_t *this); 

extern int fifo_get_char(fifo_t *this);
extern void fifo_unget_char(fifo_t *this, char c);
extern void fifo_put_char(fifo_t *this, char c);
extern int fifo_unput_char(fifo_t *this);

extern int fifo_read(fifo_t *this, void *buffer, int size);
extern int fifo_write(fifo_t *this, void *buffer, int size);

#endif
