#ifndef _UTILS_MATH_H
#define _UTILS_MATH_H

static inline long min(long x, long y)
{
    return x < y ? x : y;
}

static inline long max(long x, long y)
{
    return x > y ? x : y;
}

static inline long round_up(long size, int align)
{
    return (size + align - 1) / align * align;
}

static inline long round_down(long size, int align)
{
    return size / align * align;
}

static inline int is_power2(long size)
{
    return ((size - 1) & size) == 0;
} 

#endif
