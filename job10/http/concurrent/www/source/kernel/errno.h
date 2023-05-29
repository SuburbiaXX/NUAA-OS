#ifndef _ERRNO_H
#define _ERRNO_H

#define EZERO              0  // Non-existent error
#define EPERM             -1  // Operation not permitted
#define ENOENT            -2  // No such file or directory
#define ESRCH             -3  // No such process
#define EIO               -4  // I/O error
#define ENXIO             -5  // No such device or address
#define E2BIG             -6  // Arg list too long
#define ENOEXEC           -7  // Exec format error
#define EBADF             -8  // Bad file number
#define ECHILD            -9  // No child processes
#define EAGAIN           -10  // Try again
#define ENOMEM           -11  // Out of memory
#define EACCES           -12  // Permission denied
#define EFAULT           -13  // Bad address
#define ENOTBLK          -14  // Block device required
#define EBUSY            -15  // Device or resource busy
#define EEXIST           -16  // File exists
#define EXDEV            -17  // Cross-device link
#define ENODEV           -18  // No such device
#define ENOTDIR          -19  // Not a directory
#define EISDIR           -20  // Is a directory
#define EINVAL           -21  // Invalid argument
#define ENFILE           -22  // File table overflow
#define EMFILE           -23  // Too many open files
#define ENOTTY           -24  // Not a typewriter
#define ETXTBSY          -25  // Text file busy
#define EFBIG            -26  // File too large
#define ENOSPC           -27  // No space left on device
#define ESPIPE           -28  // Illegal seek
#define EROFS            -29  // Read-only file system
#define EMLINK           -30  // Too many links
#define EPIPE            -31  // Broken pipe
#define ENAMETOOLONG     -32  // File name too long
#define ENOSYS           -33  // Function not implemented
#define ENOTEMPTY        -34  // Directory not empty

#if CONFIG_NET_SOCKET
#define ENOTSOCK         0
#endif

#endif
