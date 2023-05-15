#ifndef _DEV_TTY_H
#define _DEV_TTY_H

extern void tty_input_char(int ascii);
extern void tty_init();

#define CTRL_KEY(key) (key - 'A' + 1)
#define CTRL_A  CTRL_KEY('A') 
#define CTRL_B  CTRL_KEY('B')
#define CTRL_C  CTRL_KEY('C')  // Abort
#define CTRL_D  CTRL_KEY('D')  // End of file
#define CTRL_U  CTRL_KEY('U')  // Erase line

#endif
