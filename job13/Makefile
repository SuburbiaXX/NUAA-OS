#!/bin/sh

CC=gcc
CFLAGS=

all:
	$(CC) -c $(CFLAGS) coro.c
	$(CC) -c $(CFLAGS) cpu.S
	$(CC) $(CFLAGS) -o pc pc.c coro.o cpu.o
	$(CC) $(CFLAGS) -o pfc pfc.c coro.o cpu.o
	$(CC) $(CFLAGS) -o find find.c fs_walk.c coro.o cpu.o
	$(CC) $(CFLAGS) -o grep grep.c fs_walk.c coro.o cpu.o

clean:
	rm -f *.o pc pfc find grep
