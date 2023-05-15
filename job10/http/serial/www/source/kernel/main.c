#include <utils/root.h>
#include <dev/vga.h>
#include <dev/hd.h>
#include <dev/kbd.h>
#include <dev/tty.h>
#include <cpu/cpu.h>
#include <cpu/irq.h>
#include <mm/page.h>
#include <mm/memory.h>
#include <mm/vm.h>
#include <fs/buffer.h>
#include <fs/inode.h>
#include <kernel/clock.h>
#include <kernel/pcb.h>
#include <net/ne2k.h>
#include "idle.h"

int main()
{
    disable_all_irq();
    vga_init();
    puts("yonix start");
    page_map_init();
    alloc_memory_init();
    kernel_pgtbl_init();
    buffer_table_init();
    inode_table_init();
    clock_init();
    hd_init();
    kbd_init();
    tty_init();
    launch_idle_process();
    return 0;
}
