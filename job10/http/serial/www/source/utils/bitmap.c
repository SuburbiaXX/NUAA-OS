#include "root.h"
#include "bitmap.h"

/* return -1 if failed. */
int alloc_bit(void *bitmap, int byte_count)
{
    unsigned int *uint = bitmap;
    int uint_count;
    int i, j;

    assert(byte_count % 4 == 0);
    uint_count = byte_count / 4;
    for (i = 0; i < uint_count; i++, uint++) {
        if (*uint == 0xFFFFFFFF)
            continue;
        for (j = 0; j < 32; j++) {
            int mask = 1 << j;
            if ((*uint & mask) == 0) {
                *uint |= mask;
                return i * 32 + j;
            }
        }
    }
    return -1;
}

void free_bit(void *bitmap, int index)
{
    assert(test_bit(bitmap, index));
    clear_bit(bitmap, index);
}

void set_bit(void *bitmap, int index)
{
    char *byte = bitmap + (index >> 3);
    int mask = 1 << (index & 7);
    *byte |= mask;
}

void clear_bit(void *bitmap, int index)
{
    char *byte = bitmap + (index >> 3);
    int mask = 1 << (index & 7);
    *byte &= ~mask;
}

int test_bit(void *bitmap, int index)
{
    char *byte = bitmap + (index >> 3);
    int mask = 1 << (index & 7);
    return *byte & mask;
}

// 0123 4567 - 0123 4567 - 0123 4567 - 0123 4567
void dump_byte(char byte)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (i == 4)
            putchar(' ');
        if ((byte >> i) & 1)
            putchar('1');
        else
            putchar('0');
    }
}

void dump_bitmap(void *bitmap, int byte_count)
{
    char *byte_array;
    int i;

    byte_array = (char *) bitmap;
    for (i = 0; i < byte_count; i++) {
        dump_byte(byte_array[i]);
        if (i != byte_count - 1) {
            if ((i + 1) % 4)
                printf(" - ");
            else
                printf("\n");
        }
    }
    printf("\n");
}
