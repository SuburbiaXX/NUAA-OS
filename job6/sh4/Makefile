CC=gcc

OBJS=main.o cmd.o parse.o utest.o

all: depend sh4 

sh4: $(OBJS) 
	$(CC) -o sh4 $(OBJS)

depend:
	./gen-dep.sh > Makefile.dep

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -f sh4 *.o

include Makefile.dep
