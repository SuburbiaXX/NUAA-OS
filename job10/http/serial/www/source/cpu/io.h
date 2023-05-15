#ifndef _CPU_IO_H
#define _CPU_IO_H

extern int in_byte(int port);
extern void out_byte(int port, int value);

extern int in_word(int port);
extern void out_word(int port, int value);

extern int in_long(int port);
extern void out_long(int port, int value);

#endif
