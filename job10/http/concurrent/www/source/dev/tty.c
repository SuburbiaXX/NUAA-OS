#include <utils/root.h>
#include <kernel/file.h>
#include <utils/fifo.h>
#include <mm/memory.h>
#include <fs/inode.h>
#include "vga.h"
#include "tty.h"

static char input_buffer[128];
static fifo_t input_fifo;
static cond_t cond;

void tty_init()
{
    fifo_init(&input_fifo, input_buffer, sizeof(input_buffer));
    cond_init(&cond);
}

void tty_backspace()
{
    fifo_unput_char(&input_fifo);
    vga_put_char('\b');
}

void tty_input_char(int ascii)
{
    if (ascii == '\r') {
        fifo_put_char(&input_fifo, '\n');
        vga_put_char('\r');
        vga_put_char('\n');
        cond_broadcast(&cond);
        return;
    }

    if (ascii == CTRL_D) {
        fifo_put_char(&input_fifo, ascii);
        cond_broadcast(&cond);
        return;
    }

    if (ascii == '\b') {
        if (input_fifo.size > 0)
            tty_backspace();
        return;
    }

    if (ascii == CTRL_U) {
        while (input_fifo.size > 0)
            tty_backspace();
        return;
    }

    if (ascii == CTRL_C) {
        puts("^C");
        return;
    }
        
    fifo_put_char(&input_fifo, ascii);
    vga_put_char(ascii);
}

void tty_file_free(file_t *this)
{
    inode_free(this->inode);
}

int tty_file_read(file_t *this, void *buffer, int size)
{
    char *p = buffer;
    char *bound = buffer + size;
    
    while (p < bound) {
        int c;
        while (fifo_empty(&input_fifo))
            cond_wait(&cond);
        c = fifo_get_char(&input_fifo);
        if (c == CTRL_D)
            break;
        *p++ = c;
        if (c == '\n')
            break;
    }
    return p - (char *)buffer;
}

int tty_file_write(file_t *this, void *buffer, int size)
{
    char *p;
    char *end = buffer + size;

    for (p = buffer; p < end; p++)
        vga_put_char(*p);
    return size;
}

int tty_file_lseek(file_t *this, off_t offset, int whence)
{
    return 0;
}

int tty_file_ioctl(file_t *this, int command, ulong arg)
{
    return 0;
}

file_class_t tty_file_class = {
    .free = tty_file_free,
    .read = tty_file_read,
    .write = tty_file_write,
    .lseek = tty_file_lseek,
    .ioctl = tty_file_ioctl
};
