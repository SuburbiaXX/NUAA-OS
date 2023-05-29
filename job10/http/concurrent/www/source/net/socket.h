#ifndef _NET_SOCKET_H
#define _NET_SOCKET_H

typedef struct sock_addr {
    uint16_t family;
    uint8_t  data[14];
} sock_addr_t;

typedef struct sock_addr_in {
    uint16_t family;
    uint16_t port;
    uint32_t addr;
    uint8_t  pad[8];
} sock_addr_in_t;

struct socket_class;

#define socket_member_list              \
    struct socket_class *class;         \
    int type;                           \
    int ref_count;                      \
    int flags;                          \
    off_t offset;                       \
    void *pointer

typedef struct socket {
    socket_member_list;
} socket_t;

typedef struct socket_class {
    void (*release)(socket_t *this);
    int (*read)(socket_t *this, void *buf, int size);
    int (*write)(socket_t *this, void *buf, int size);
    int (*lseek)(socket_t *this, off_t offset, int whence);
    int (*ioctl)(socket_t *this, int request, ulong arg);
    int (*bind)(socket_t *this, sock_addr_t *addr, int addr_len);
    int (*connect)(socket_t *this, sock_addr_t *addr, int addr_len);
    int (*listen)(socket_t *this, int backlog);
    int (*accept)(socket_t *this, sock_addr_t *addr, int *addr_len);
    int (*getsockname)(socket_t *this, sock_addr_t *name, int *name_len);
    int (*getpeername)(socket_t *this, sock_addr_t *name, int *name_len);
    int (*send)(socket_t *this, void *buf, int len, int flags);
    int (*recv)(socket_t *this, void *buf, int len, int flags);
    int (*sendto)(socket_t *this, void *buf, int len, int flags, 
        sock_addr_t *addr, int addr_len);
    int (*recvfrom)(socket_t *this, void *buf, int len, int flags, 
        sock_addr_t *addr, int *addr_len);
    int (*shutdown)(socket_t *this, int how);
} socket_class_t;

#endif
