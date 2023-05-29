#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3

/* These constants define the various ELF target machines */
#define EM_NONE  0
#define EM_M32   1
#define EM_SPARC 2
#define EM_386   3
#define EM_486   6

#define EI_IDENT_NUMBER         16
typedef struct {
    uchar   ident[EI_IDENT_NUMBER];
    ushort  type;
    ushort  machine;
    ulong   version;
    ulong   entry;
    ulong   program_head_offset;
    ulong   section_head_offset;
    ulong   flags;
    ushort  elf_head_size;
    ushort  program_head_entry_size;
    ushort  program_head_number;
    ushort  section_head_entry_size;
    ushort  section_head_number;
    ushort  section_head_string_index;
} elf_head_t;

#define ELF_READ     0x4
#define ELF_WRITE    0x2
#define ELF_EXECUTE  0x1

typedef struct {
    ulong   type;
    ulong   offset;
    ulong   virtual_address;
    ulong   physical_address;
    ulong   file_size;
    ulong   memory_size;
    ulong   flags;
    ulong   align;
} program_head_t;

#define EI_MAG0         0       /* ident[] indexes */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_PAD          7

#define ELF_MAG0        0x7f    /* EIMAG */
#define ELF_MAG1        'E'
#define ELF_MAG2        'L'
#define ELF_MAG3        'F'
#define ELF_MAG         "\177ELF"

#define ELF_CLASSNONE   0       /* EICLASS */
#define ELF_CLASS32     1
#define ELF_CLASS64     2
#define ELF_CLASSNUM    3

#endif 
