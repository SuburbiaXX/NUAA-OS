#ifndef _KERNEL_IDLE_H
#define _KERNEL_IDLE_H

#include <mm/page.h>

#define IDLE_CODE_START 0x1000
#define IDLE_CODE_END   (IDLE_CODE_START + PAGE_SIZE)

#define IDLE_DATA_START 0x2000
#define IDLE_DATA_END   (IDLE_DATA_START + PAGE_SIZE)

#define IDLE_STACK_START 0x3000
#define IDLE_STACK_END  (IDLE_STACK_START + PAGE_SIZE)

extern void launch_idle_process();
#endif
