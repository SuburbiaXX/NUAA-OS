#include <utils/root.h>
#include <kernel/file.h>
#include <kernel/errno.h>
#include <mm/vm.h>
#include "socket.h"

static int get_socket(int fd, socket_t **socket)
{
    file_t *file;

    file = get_file(fd);
    if (!file)
        return -EBADF;
    if (file->type != FILE_SOCKET)
        return -ENOTSOCK;
    *socket = (socket_t *)file;
    return 0;
}

int sys_socket(int domain, int type, int protocol)
{
    return 0;
}

int sys_bind(int fd, sock_addr_t *addr, int addr_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;

    return socket->class->bind(socket, addr, addr_len);
}

int sys_connect(int fd, sock_addr_t *addr, int addr_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_read(addr, addr_len))
        return error;

    return socket->class->connect(socket, addr, addr_len);
}

int sys_listen(int fd, int backlog)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;

    return socket->class->listen(socket, backlog);
}

int sys_accept(int fd, sock_addr_t *addr, int *addr_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;

    return socket->class->accept(socket, addr, addr_len);
}

int sys_getsockname(int fd, sock_addr_t *name, int *name_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_write(name, *name_len))
        return error;
    return socket->class->getsockname(socket, name, name_len);
}

int sys_getpeername(int fd, sock_addr_t *name, int *name_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_write(name, *name_len))
        return error;
    return socket->class->getpeername(socket, name, name_len);
}

/*
int sys_socketpair(int domain, int type, int protocol, int fd[2])
{       
    if ((domain < 1) || (domain > PFEND))
        return EINVAL; 
    if (!(createsocket[domain].socketpair))
        return EPROTONOSUPPORT;
    return createsocket[domain].socketpair(domain, type, protocol, fd);
}
*/

int sys_send(int fd, void *buf, int len, int flags)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_read(buf, len))
        return error;

    return socket->class->send(socket, buf, len, flags);
}

int sys_recv(int fd, void *buf, int len, int flags)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_write(buf, len))
        return error;

    return socket->class->recv(socket, buf, len, flags);
}

int sys_sendto(int fd, void *buf, int len, int flags, 
               sock_addr_t *addr, int addr_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_read(buf, len))
        return error;
    if (error = verify_read(addr, addr_len))
        return error;

    return socket->class->sendto(socket, buf, len, flags, addr, addr_len);
}

int sys_recvfrom(int fd, void *buf, int len, int flags, 
                 sock_addr_t *addr, int *addr_len)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    if (error = verify_write(buf, len))
        return error;
    if (error = verify_write(addr, *addr_len))
        return error;
    return socket->class->recvfrom(socket, buf, len, flags, addr, addr_len);
}

int sys_shutdown(int fd, int how)
{
    int error;
    socket_t *socket;

    if (error = get_socket(fd, &socket))
        return error;
    
    return socket->class->shutdown(socket, how);
}

int sys_setsockopt(int fd, int level, int optname, void * optval, 
        int optlen)
{
    return 0;
}

int sys_getsockopt(int fd, int level, int optname, void * optval,
        int * optlen)
{
    return 0;
}
