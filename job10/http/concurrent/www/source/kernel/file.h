#ifndef _KERNEL_FILE_H
#define _KERNEL_FILE_H

#include "limits.h"
#include "fcntl.h"

enum {
    FILE_INODE,
    FILE_PIPE,
    FILE_DEVICE,
    FILE_SOCKET,
};

struct file;
struct pipe;
typedef struct file file_t; 

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct {
    void (*free)(file_t *this);
    int (*read)(file_t *this, void *buffer, int size);
    int (*write)(file_t *this, void *buffer, int size);
    int (*lseek)(file_t *this, off_t offset, int whence);
    int (*ioctl)(file_t *this, int request, ulong arg);
} file_class_t;

struct file {
    file_class_t *class;
    int type;
    int ref_count;
    int flags;
    off_t offset;
    union {
        struct inode *inode;
        struct pipe *pipe;
        void *pointer;
    };
};

extern file_t *get_file(int fd);
extern file_t *file_new(int type, int flags, void *pointer);
extern void file_delete(file_t *this);
extern void file_hold(file_t *this);
extern void file_free(file_t *this);
extern int file_permit_read(file_t *this);
extern int file_permit_write(file_t *this);

extern void close_all_file();
extern int alloc_fd(int *fd);
extern int alloc_fd_pair(int fd[2]);
extern void install_file(int fd, file_t *file);

#endif
