#ifndef _KERNEL_WAIT_H
#define _KERNEL_WAIT_H

/* The <sys/wait.h> header contains macros related to wait(). The value
 * returned by wait() and waitpid() depends on whether the process 
 * terminated by an exit() call, was killed by a signal, or was stopped
 * due to job control, as follows:
 *
 *                               High byte   Low byte
 *                              +---------------------+
 *      exit(status)            |  status  |    0     |
 *                              +---------------------+
 *      killed by signal        |    0     |  signal  |
 *                              +---------------------+
 *      stopped (job control)   |  signal  |   0177   |
 *                              +---------------------+
 */
#define WIFEXITED(s)    ((s) & 0xff == 0)               /* normal exit */
#define WEXITSTATUS(s)  (((s) >> 8) & 0xff)             /* exit status */
#define WIFSIGNALED(s)  (((s) >> 8) == 0)
#define WTERMSIG(s)     ((s) & 0xff)

#endif
