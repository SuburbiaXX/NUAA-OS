#ifndef _DEV_NUMBER_H
#define _DEV_NUMBER_H

#define MAJOR_MEMORY        1
#define MAJOR_TTY           2
#define MAJOR_FLOPPY_DISK   3
#define MAJOR_HARD_DISK     4
#define MAJOR_RAM_DISK      5

#define MAX_BLOCK_DEVICE    6
#define MAX_CHAR_DEVICE     6
static inline int device_make(int major, int minor)
{
    return (major << 8) + minor;
}

static inline int device_major(int device)
{
    return device >> 8 & 0xff;
}

static inline int device_minor(int device)
{
    return device & 0xff;
}

struct file;
extern int open_device_file(int device, struct file *file);

#endif
