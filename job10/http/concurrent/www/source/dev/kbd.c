#include <utils/root.h>
#include <cpu/io.h>
#include <cpu/irq.h>
#include "tty.h"
#include "kbd.h"

#define NUL 0

static char normal_map[256] = {
  /* 0x00 */   NUL,  033,  '1',  '2',  '3',  '4',  '5',  '6', 
  /* 0x08 */   '7',  '8',  '9',  '0',  '-',  '=', '\b', '\t', 
  /* 0x10 */   'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 
  /* 0x18 */   'o',  'p',  '[',  ']', '\r',  NUL,  'a',  's', 
  /* 0x20 */   'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';', 
  /* 0x28 */  '\'',  '`',  NUL, '\\',  'z',  'x',  'c',  'v', 
  /* 0x30 */   'b',  'n',  'm',  ',',  '.',  '/',  NUL,  '*', 
  /* 0x38 */   NUL,  040,  NUL, 
};

static char shift_map[256] = {
  /* 0x00 */   NUL,  033,  '!',  '@',  '#',  '$',  '%',  '^', 
  /* 0x08 */   '&',  '*',  '(',  ')',  '_',  '+', '\b', '\t', 
  /* 0x10 */   'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I', 
  /* 0x18 */   'O',  'P',  '{',  '}', '\r',  NUL,  'A',  'S', 
  /* 0x20 */   'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':', 
  /* 0x28 */   '"',  '~',  NUL,  '|',  'Z',  'X',  'C',  'V', 
  /* 0x30 */   'B',  'N',  'M',  '<',  '>',  '?',  NUL,  '*', 
  /* 0x38 */   NUL,  040,  NUL, 
};

static int kbd_key;
static int ctrl_pressing;
static int shift_pressing;
static int alt_pressing;
static int capslock;

#define KEY_ALT         0x38
#define KEY_CTRL        0x1d
#define KEY_LEFT_SHIFT  0x2a
#define KEY_RIGHT_SHIFT 0x36
#define KEY_CAPSLOCK    0x3a

int key_up(int key)
{
    return (key & 0x80) == 0x80;
}

int key_down(int key)
{
    return (key & 0x80) == 0x00;
}

int key_clear(int key)
{
    return key & 0x7F;
}

bool handle_special_key(int key)
{
    int code = key_clear(key);
    int pressing = key_down(key);

    switch (code) {
        case KEY_ALT:
            alt_pressing = pressing;
            break;

        case KEY_CTRL:
            ctrl_pressing = pressing;
            break;

        case KEY_LEFT_SHIFT:
        case KEY_RIGHT_SHIFT:
            shift_pressing = pressing;
            break;

        case KEY_CAPSLOCK:
            if (pressing)
                capslock = !capslock;
            break;

        default:
            return false;
    }
    return true;
}

static bool is_alpha(int ascii)
{
    return (ascii >= 'A' && ascii <= 'Z') || (ascii >= 'a' && ascii <= 'z');
}

void handle_normal_key(uint key)
{
    uint ascii;

    if (key_up(key))
        return;
    key = key_clear(key);
    ascii = normal_map[key];
    if (ascii == NUL)
        return;

    // Ctrl-A => 1
    // Ctrl-B => 2
    // Ctrl-C => 3
    // Ctrl-D => 4
    // ...
    if (ctrl_pressing) {
        ascii = shift_map[key];
        ascii = ascii - 'A' + 1;
        tty_input_char(ascii);
        return;
    }

    if (shift_pressing) {
        ascii = shift_map[key];
        tty_input_char(ascii);
        return;
    }

    if (capslock) {
        ascii = normal_map[key];
        if (is_alpha(ascii))
            ascii = shift_map[key];
        tty_input_char(ascii);
        return;
    }

    tty_input_char(ascii);
}

void kbd_isr(int irq_no)
{
    int stat = in_byte(KBD_STAT);
    if ((stat & KBD_STAT_FULL) == 0)
        return;
    kbd_key = in_byte(KBD_DATA);
}

void kbd_soft_isr(int irq_no)
{
    if (!handle_special_key(kbd_key))
        handle_normal_key(kbd_key);
}

void kbd_init()
{
    register_irq(IRQ_KBD, kbd_isr);
    register_soft_irq(IRQ_KBD, kbd_soft_isr);
}
