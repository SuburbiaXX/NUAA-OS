#include <utils/root.h>
#include "tcp.h"
#include "packet.h"

void tcp_socket_release(socket_t *self)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
}

int tcp_socket_read(socket_t *self, void *buf, int size)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_write(socket_t *self, void *buf, int size)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_lseek(socket_t *self, off_t offset, int whence)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_ioctl(socket_t *self, int request, ulong arg)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_bind(socket_t *self, sock_addr_t *addr, int addr_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_connect(socket_t *self, sock_addr_t *addr, int addr_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_listen(socket_t *self, int backlog)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_accept(socket_t *self, sock_addr_t *addr, int *addr_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_getsockname(socket_t *self, sock_addr_t *name, int *name_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_getpeername(socket_t *self, sock_addr_t *name, int *name_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_send(socket_t *self, void *buf, int len, int flags)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_recv(socket_t *self, void *buf, int len, int flags)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_sendto(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int addr_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_recvfrom(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int *addr_len)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

int tcp_socket_shutdown(socket_t *self, int how)
{
    tcp_socket_t *this = (tcp_socket_t *)self;
    return 0;
}

void tcp_socket_input(tcp_socket_t *self, packet_t *packet)
{
}

void tcp_input(packet_t *packet)
{
}    
