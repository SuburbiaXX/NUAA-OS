#ifndef _DEV_HD_H
#define _DEV_HD_H

#define IDE_DATA          0x1f0 
#define IDE_ERROR         0x1f1
#define IDE_SECTOR_COUNT  0x1f2
#define IDE_LBA_0700      0x1f3
#define IDE_LBA_1508      0x1f4
#define IDE_LBA_2316      0x1f5
#define IDE_LBA_2724      0x1f6 // 101dhhhh, d=drive, hhhh=2724
#define IDE_STATUS        0x1f7  
#define IDE_COMMAND       0x1f7 // same io address, read=status, write=command
#define IDE_CONTROL       0x3f6

#define IDE_STATUS_ERROR  0x01
#define IDE_STATUS_DRQ    0x08
#define IDE_STATUS_BUSY   0x80

#define IDE_READ          0x20
#define IDE_WRITE         0x30
#define SECTOR_SIZE       512

#define IO_READ           IDE_READ
#define IO_WRITE          IDE_WRITE
#define IO_NONE           -1

extern void hd_init();
struct buffer;
extern void hd_request(struct buffer *buffer, int command);

#endif
