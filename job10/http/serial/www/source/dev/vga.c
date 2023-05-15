#include <utils/root.h>
#include <cpu/io.h>
#include <cpu/layout.h>
#include <mm/page.h>
#include "vga.h"
#include "tty.h"

#define vga_row_number 25
#define vga_column_number 80
vga_char_t *vga_base;
vga_char_t *vga_bound;
vga_char_t *vga_cursor;

void vga_char_set(vga_char_t *this, char value)
{
    this->value = value;
    this->attr = VGA_NORMAL;
}

void vga_update_cursor()
{
    int offset = vga_cursor - vga_base;
    out_byte(VGA_ADDRESS, VGA_CURSOR_LOW);
    out_byte(VGA_VALUE, (offset>>8)&0xff);
    out_byte(VGA_ADDRESS, VGA_CURSOR_HIGH);
    out_byte(VGA_VALUE, offset&0xff);
}

void vga_init()
{
    vga_char_t *vga_char;

    vga_base = (vga_char_t *) to_vaddr(0xB8000);
    vga_bound = vga_base + vga_row_number * vga_column_number;

    for (vga_char = vga_base; vga_char < vga_bound; vga_char++)
        vga_char_set(vga_char, ' ');

    vga_cursor = vga_base;
    vga_update_cursor();
}

void vga_scroll_up()
{
    vga_char_t *source;
    vga_char_t *target;
    int count;

    if (vga_cursor < vga_bound) 
        return;

    source = vga_base + vga_column_number;
    target = vga_base;
    count = (vga_row_number - 1) * vga_column_number;
    for (; count--; source++, target++)
        *target = *source;
    for (; target < vga_bound; target++)
        vga_char_set(target, ' ');

    vga_cursor -= vga_column_number;
}

void vga_locate_cursor(int row, int column)
{
    vga_cursor = vga_base + row * vga_column_number + column;
}

void vga_put_char(char c)
{
    int row, column;

    row = (vga_cursor - vga_base) / vga_column_number;
    column = (vga_cursor - vga_base) % vga_column_number;
    switch (c) {
        case '\t':
            column = (column + 8) & ~7;  
            vga_locate_cursor(row, column);
            break;

        case '\n':
            row++;
            column = 0;
            vga_locate_cursor(row, column);
            break;

        case '\r':
            column = 0;
            vga_locate_cursor(row, column);
            break;

        case '\b':
            if (column > 0) {
                column -= 1;
                vga_locate_cursor(row, column);
                vga_char_set(vga_cursor, ' ');
            }
            break;

        default:
            vga_char_set(vga_cursor, c);
            vga_cursor++;
    }

    vga_scroll_up();
    vga_update_cursor();
}

void vga_write(char *buffer, int size)
{
    int i;

    for (i = 0; i < size; i++)
        vga_put_char(buffer[i]);
}

void vga_puts(char *string)
{
    char c;

    for (; c = *string; string++)
        vga_put_char(c);
    vga_put_char('\n');
}
