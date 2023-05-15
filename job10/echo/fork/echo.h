#ifndef _ECHO_H
#define _ECHO_H

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define error(message)      \
    do {                    \
        perror(message);    \
        exit(EXIT_FAILURE); \
    } while (0)

#endif
