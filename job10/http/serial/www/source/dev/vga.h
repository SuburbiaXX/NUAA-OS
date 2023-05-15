#ifndef _DEVICE_VGA_H
#define _DEVICE_VGA_H

#define VGA_ADDRESS 0x3d4
#define VGA_VALUE 0x3d5
#define VGA_BASE_LOW 12
#define VGA_BASE_HIGH 13
#define VGA_CURSOR_LOW 14 
#define VGA_CURSOR_HIGH 15 

#define VGA_BLINK   0x80
#define VGA_LIGHT   0x08
#define VGA_RED     0x04
#define VGA_GREEN   0x02
#define VGA_BLUE    0x01
#define VGA_WHITE   0x07
#define VGA_BLACK   0x00
#define VGA_YELLOW  0x06

#define VGA_ATTR(blink, light, fg, bg) (blink+(bg<<4)+light+fg)
#define VGA_NORMAL VGA_ATTR(0, 0, VGA_WHITE, VGA_BLACK)
#define VGA_ERROR VGA_ATTR(0, 0, VGA_WHITE, VGA_BLUE)

typedef struct {
    char value;
    char attr;
} vga_char_t;

extern void vga_init();
extern void vga_clear_screen();
extern void vga_put_char(char c);
extern void vga_write(char *buffer, int size);
extern void vga_puts(char *string);

#endif
