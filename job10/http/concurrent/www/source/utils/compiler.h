#ifndef _UTILS_COMPILER_H
#define _UTILS_COMPILER_H

#define offset_of(type, member) ((ulong) &((type *)0)->member)
#define asm_link __attribute__((regparm(0)))
#define gcc_packed __attribute__((packed))
#define gcc_aligned(number) __attribute__((aligned(number)))
#define gcc_printf(m, n) __attribute__((format(printf,m,n)))

#endif
