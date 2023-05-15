#ifndef _UTILS_ROOT_H
#define _UTILS_ROOT_H

#define K 1024UL
#define M (1024*1024UL)
#define G (1024*1024*1024UL)

#include "config.h"
#include "types.h"
#include "compiler.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "list.h"

#define NULL 0
typedef int bool;
#define true 1
#define false 0
#define assert(express)                                                 \
    do {                                                                \
        if (!(express))                                                 \
            panic("%s:%d:Assertion failed!\n", __FILE__, __LINE__);     \
    } while (0)

#define raise(code)                         \
    do {                                    \
        error = code;                       \
        goto bad;                           \
    } while (0)

extern int signal;
#define trace() printf("%s:%d\n", __FILE__, __LINE__)
#endif
