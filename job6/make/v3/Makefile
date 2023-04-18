OBJS=main.o min.o max.o

all: $(OBJS)
	cc -o app $(OBJS)

%.o: %.c
	cc -c -o $@ $<

clean:
	rm -f app *.o

include Makefile.dep
