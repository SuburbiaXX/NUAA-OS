#include <utils/root.h>
#include "udp.h"
#include "packet.h"

void udp_socket_release(socket_t *self)
{
    udp_socket_t *this = (udp_socket_t *)self;
}

int udp_socket_read(socket_t *self, void *buf, int size)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_write(socket_t *self, void *buf, int size)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_lseek(socket_t *self, off_t offset, int whence)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_ioctl(socket_t *self, int request, ulong arg)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_bind(socket_t *self, sock_addr_t *addr, int addr_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_connect(socket_t *self, sock_addr_t *addr, int addr_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_listen(socket_t *self, int backlog)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_accept(socket_t *self, sock_addr_t *addr, int *addr_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_getsockname(socket_t *self, sock_addr_t *name, int *name_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_getpeername(socket_t *self, sock_addr_t *name, int *name_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_send(socket_t *self, void *buf, int len, int flags)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_recv(socket_t *self, void *buf, int len, int flags)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_sendto(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int addr_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_recvfrom(socket_t *self, void *buf, int len, int flags, 
    sock_addr_t *addr, int *addr_len)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

int udp_socket_shutdown(socket_t *self, int how)
{
    udp_socket_t *this = (udp_socket_t *)self;
    return 0;
}

void udp_socket_input(udp_socket_t *self, packet_t *packet)
{
}

void udp_input(packet_t *packet)
{
}    
