#include <utils/root.h>
#include <kernel/file.h>
#include "number.h"

extern file_class_t tty_file_class;

int open_device_file(int device, file_t *file)
{
    int major;

    major = device_major(device);
    switch (major) {
        case MAJOR_MEMORY:
            break;

        case MAJOR_TTY:
            file->class = &tty_file_class;
            break;

        case MAJOR_FLOPPY_DISK:
            break;

        case MAJOR_HARD_DISK:
            break;

        case MAJOR_RAM_DISK:
            break;

    }
    return 0;
}
