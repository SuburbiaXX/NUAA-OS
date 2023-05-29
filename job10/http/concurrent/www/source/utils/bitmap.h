#ifndef _UTILS_BITMAP_H
#define _UTILS_BITMAP_H

extern int alloc_bit(void *bitmap, int byte_count);
extern void free_bit(void *bitmap, int index);
extern void set_bit(void *bitmap, int index);
extern void clear_bit(void *bitmap, int index);
extern int test_bit(void *bitmap, int index);
extern void dump_bitmap(void *bitmap, int byte_count);

#endif
