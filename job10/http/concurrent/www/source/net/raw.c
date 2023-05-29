#include <utils/root.h>
#include "raw.h"
#include "packet.h"

void raw_socket_release(socket_t *self)
{
    raw_socket_t *this = (raw_socket_t *)self;
}

int raw_socket_read(socket_t *self, void *buf, int size)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_write(socket_t *self, void *buf, int size)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_lseek(socket_t *self, off_t offset, int whence)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_ioctl(socket_t *self, int request, ulong arg)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_bind(socket_t *self, sock_addr_t *addr, int addr_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_connect(socket_t *self, sock_addr_t *addr, int addr_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_listen(socket_t *self, int backlog)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_accept(socket_t *self, sock_addr_t *addr, int *addr_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_getsockname(socket_t *self, sock_addr_t *name, int *name_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_getpeername(socket_t *self, sock_addr_t *name, int *name_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_send(socket_t *self, void *buf, int len, int flags)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_recv(socket_t *self, void *buf, int len, int flags)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_sendto(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int addr_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_recvfrom(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int *addr_len)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

int raw_socket_shutdown(socket_t *self, int how)
{
    raw_socket_t *this = (raw_socket_t *)self;
    return 0;
}

void raw_socket_input(raw_socket_t *self, packet_t *packet)
{
}

void raw_input(packet_t *packet)
{
}    
