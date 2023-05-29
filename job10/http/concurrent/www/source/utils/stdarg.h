#ifndef _UTILS_STDARG_H
#define _UTILS_STDARG_H

typedef char *va_list;

#define __va_rounded_size(type)                                           \
    (((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, last_arg) 						  \
    (ap = ((char *)&(last_arg) + __va_rounded_size(last_arg)))

#define va_end(ap)

#define va_arg(ap, type)						  \
    (ap += __va_rounded_size(type),					  \
    *((type *)(ap - __va_rounded_size(type))))

#endif
