#ifndef _KERNEL_FCNTL_H
#define _KERNEL_FCNTL_H

#define O_ACCMODE     0003
#define O_RDONLY        00
#define O_WRONLY        01
#define O_RDWR          02
#define O_CREAT       0100  /* not fcntl */
#define O_EXCL        0200  /* not fcntl */
#define O_NOCTTY      0400  /* not fcntl */
#define O_TRUNC      01000  /* not fcntl */
#define O_APPEND     02000
#define O_NONBLOCK   04000

#endif
