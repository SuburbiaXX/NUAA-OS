#ifndef _DEV_KBD_H
#define _DEV_KBD_H

#define KBD_DATA 0x60
#define KBD_STAT 0x64
#define KBD_STAT_FULL 1
    
extern void kbd_init();
#endif
