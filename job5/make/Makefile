CC           := gcc
LD           := gcc
CC           := clang
LD           := clang

TOP_DIR      := $(shell pwd)
TOP_DIR	     := .
CFLAGS       := -c -I$(TOP_DIR)
CFLAGS       += -Wall -Wundef -Wno-unused -Wno-unused-result -Wno-parentheses
CFLAGS       += -Wno-pointer-arith -Wno-pointer-sign -Wno-int-to-pointer-cast
CFLAGS       += -Wfatal-errors -Werror
CFLAGS       += -fno-common

DEBUG        := 1
ifeq ($(DEBUG), 1)
CFLAGS       += -fsanitize=address
CFLAGS 	     += -g
LDFLAGS      += -fsanitize=address
else
CFLAGS       += -O -DNDEBUG -fno-omit-frame-pointer
endif

SRCS         := $(wildcard *.c)
OBJS         := $(subst .c,.o,$(SRCS))

TARGET_ELF   := app

all: $(OBJS)
	@echo "LD -o $(TARGET_ELF)"
	@$(LD) -o $(TARGET_ELF) $(OBJS) $(LDFLAGS) -lpthread

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -o $@ $<

dep:
	@(for SRC in $(SRCS); do 		 				\
	    OBJ=`echo $$SRC | sed -e 's/\.c/\.o/'`; 	\
	    cpp -I$(TOP_DIR) -MM -MT $$OBJ $$SRC;  		\
	done) >Makefile.dep

clean:
	@rm -f $(OBJS) $(TARGET_ELF) tags core Makefile.dep

ifeq (Makefile.dep, $(wildcard Makefile.dep))
include Makefile.dep
endif
