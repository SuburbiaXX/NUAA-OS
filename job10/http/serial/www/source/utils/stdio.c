#include <utils/root.h>
#include <dev/vga.h>
#include "stdio.h"
#include "stdarg.h"

static char *print_string(char *p, char *string)
{
    while (*string)
        *p++ = *string++;
    return p;
}

static char *print_unsigned(char *p, int base, unsigned number)
{
    char *digits = "0123456789ABCDEF";
    char stack[32];
    int top = 0;

    do {
        stack[top++] = digits[number % base]; 
        number = number / base;
    } while (number);
    while (top--)
        *p++ = stack[top];

    return p;
}

static char *print_signed(char *p, int base, int number)
{
    if (number < 0) {
        number = -number;
        *p++ = '-';
    }
    p = print_unsigned(p, base, number);
    return p;
}

int vsprintf(char *buffer, char *format, va_list ap)
{
    char *p;
    int number;
    char *string;

    p = buffer;
    while (*format) {
        if (*format != '%') {
            *p++ = *format++;
            continue;
        }

        switch (format[1]) {
            case '%':
                *p++ = '%';
                break;

            case 'c':
                number = va_arg(ap, int);
                *p++ = (char) number;
                break;

            case 's':
                string = va_arg(ap, char*);
                p = print_string(p, string);
                break;

            case 'b':
                number = va_arg(ap, int);
                p = print_unsigned(p, 2, number);
                break;

            case 'u':
                number = va_arg(ap, int);
                p = print_unsigned(p, 10, number);
                break;

            case 'd':
                number = va_arg(ap, int);
                p = print_signed(p, 10, number);
                break;

            case 'p':
                *p++ = '0';
                *p++ = 'x';
            case 'x':
                number = va_arg(ap, int);
                p = print_unsigned(p, 16, number);
                break;
        }
        format += 2;
    }
    *p = 0;
    return p - buffer;
}

int sprintf(char *buffer, char *format, ...)
{
    va_list ap;
    int size;

    va_start(ap, format);
    size = vsprintf(buffer, format, ap); 
    va_end(ap);
    return size;
}

int vprintf(char *format, va_list ap)
{
    static char buffer[256];
    int size;
    
    size = vsprintf(buffer, format, ap);
    vga_write(buffer, size);
    return size;
}

int printf(char *format, ...)
{
    va_list ap; 
    int size;
    
    va_start(ap, format);
    size = vprintf(format, ap);
    va_end(ap);
    return size;
}

void panic(char *format, ...)
{
    va_list ap; 
    
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);

    while (1)
        ;
}

int puts(char *string)
{
    vga_puts(string);
    return 0;
}

int putchar(int c)
{
    vga_put_char(c);
    return 0;
}

void printf_test()
{
    printf("%%c='%c' %%s=\"%s\"\n", 'c', "string");
    printf("%%d=%d %%u=%u\n", 1, 1);
    printf("%%d=%d %%u=%u\n", -1, -1);
    printf("%%x=%x %%p=%p\n", 256, (void *)4096);
}
