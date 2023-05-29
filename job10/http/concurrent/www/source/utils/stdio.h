#ifndef _UTILS_STDIO_H
#define _UTILS_STDIO_H

extern int sprintf(char *buffer, char *format, ...) gcc_printf(2,3);
extern int printf(char *format, ...);
extern void panic(char *format, ...) gcc_printf(1,2);
extern int puts(char *string);
extern int putchar(int c);
extern void printf_test();

#endif
