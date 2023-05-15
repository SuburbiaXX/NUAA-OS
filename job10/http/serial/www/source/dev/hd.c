#include <utils/root.h>
#include <cpu/io.h>
#include <cpu/irq.h>
#include <fs/buffer.h>
#include "hd.h"

static cond_t cond;
static buffer_t *this_buffer;
static int this_command;

static void read_sector(void *buffer)
{
    uint16_t *word = buffer;
    uint16_t *end = buffer + SECTOR_SIZE;

    for (; word < end; word++)
        *word = in_word(IDE_DATA);
}

static void write_sector(void *buffer)
{
    short *word = buffer;
    short *end = buffer + SECTOR_SIZE;
   
    for (; word < end; word++)
        out_word(IDE_DATA, *word);
}

int check_error()
{
    int status = in_byte(IDE_STATUS);
    return status & IDE_STATUS_ERROR;
}

static void wait_data_request()
{
    while (1) {
        int status = in_byte(IDE_STATUS);
        if (status & IDE_STATUS_DRQ)
            return;
    }
}

static void wait_ready()
{
    while (1) {
        int status = in_byte(IDE_STATUS);
        if ((status & IDE_STATUS_BUSY) == 0)
            return;
    }
}

void hd_command(int command, int drive, int start_sector, int sector_count)
{
    int byte;

    wait_ready();
    out_byte(IDE_CONTROL, 0);

    byte = start_sector & 0xff;
    out_byte(IDE_LBA_0700, byte);

    byte = (start_sector>>8) & 0xff;
    out_byte(IDE_LBA_1508, byte);

    byte = (start_sector>>16) & 0xff;
    out_byte(IDE_LBA_2316, byte);

    byte = 0xE0 | (drive<<4) | ((start_sector>>24)&0x0f);
    out_byte(IDE_LBA_2724, byte);
    out_byte(IDE_SECTOR_COUNT, sector_count);

    assert((command == IDE_READ) || (command == IDE_WRITE));
    out_byte(IDE_COMMAND, command);
}

void hd_isr(int irq_no)
{
    if (this_buffer == NULL)
        return;
    if (this_command == IO_READ)
        read_sector(this_buffer->data);
}

void hd_soft_isr(int irq_no)
{
    if (this_buffer == NULL)
        return;
    buffer_end_io(this_buffer, BUFFER_CLEAN);
    this_buffer = NULL;
    this_command = IO_NONE;
    cond_signal(&cond);
}

void hd_request(buffer_t *buffer, int command)
{
    while (this_buffer)
        cond_wait(&cond);
    this_buffer = buffer;
    this_command = command;

    hd_command(command, 0, buffer->block, 1);
    if (command == IO_WRITE) {
        wait_data_request();
        write_sector(buffer->data);
    }
}

void hd_init()
{
    puts("init hd");
    cond_init(&cond);
    this_buffer = NULL;
    this_command = IO_NONE;
    register_irq(IRQ_HD, hd_isr);
    register_soft_irq(IRQ_HD, hd_soft_isr);
}
