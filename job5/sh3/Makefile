run: cmd.o parse.o main.o
		cc -o sh3 cmd.o parse.o main.o

main.o: main.c parse.h cmd.h
		cc -c main.c parse.h cmd.h

parse.o: parse.c parse.h
		cc -c parse.c parse.h

cmd.o: cmd.c cmd.h
		cc -c cmd.c cmd.h

clean:
	rm -f sh3 *.o *.h.gch
