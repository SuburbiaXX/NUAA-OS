#ifndef _TCP_H
#define _TCP_H

extern int tcp_listen(char *ip_address, int port);
extern int tcp_accept(int server_fd);

#endif
