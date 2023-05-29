CC           := gcc -m32
AS           := nasm -f elf
LD           := ld -m elf_i386 -T Linker.ld

TOP_DIR      := $(shell pwd)
CFLAGS       := -c -std=gnu99 -nostdinc -I$(TOP_DIR)
CFLAGS       += -Wall -Wno-parentheses -Wno-pointer-arith -Wno-unused -Wundef
CFLAGS       += -Wfatal-errors -Werror
CFLAGS       += -fno-stack-protector
DEBUG        := 0
ifeq ($(DEBUG), 1)
CFLAGS 	     += -g
else
CFLAGS       += -O -DNDEBUG -fomit-frame-pointer
endif

SRCS         := $(wildcard cpu/*.asm cpu/*.c)
SRCS         += $(wildcard dev/*.c)
SRCS         += $(wildcard utils/*.c)
SRCS         += $(wildcard kernel/*.c)
SRCS         += $(wildcard mm/*.c)
SRCS         += $(wildcard fs/*.c)
OBJS         := $(subst .asm,.o,$(subst .c,.o,$(SRCS)))

TARGET_ELF   := boot/yonix.elf
TARGET_IMG   := boot/yonix.img
FLOPPY_IMG   := boot/floppy.img

all: $(OBJS)
	@set -e
	@make -C boot
	$(LD) -o $(TARGET_ELF) $(OBJS)
	objcopy -O binary $(TARGET_ELF) $(TARGET_IMG)
	cat boot/boot.img $(TARGET_IMG) >$(FLOPPY_IMG)
	truncate --size=1440K $(FLOPPY_IMG) 

%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	@echo "  AS $<"
	@$(AS) -o $@ $< 

dep:
	@set -e
	@(for SRC in $(SRCS); do \
	    OBJ=`echo $$SRC | sed -e 's/\(\.c\|\.asm\)/\.o/'` ; \
	    cpp -I$(TOP_DIR) -MM -MT $$OBJ $$SRC; \
	done) >Makefile.dep

clean:
	@set -e
	@rm -f $(OBJS) $(TARGET_ELF) $(TARGET_IMG) boot/floppy.img tags Makefile.dep

ax:
	addr2line -e $(TARGET_ELF) 0xc0012366

count:
	wc -l `find $(SRCS) -name '*.[ch]' -or -name '*.asm'`

ifeq (Makefile.dep, $(wildcard Makefile.dep))
include Makefile.dep
endif
