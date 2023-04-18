all: main.o min.o max.o
	cc -o app main.o min.o max.o 

main.o: main.c min.h max.h
	cc -c main.c

min.o: min.c min.h
	cc -c min.c

max.o: max.c max.h
	cc -c max.c

clean:
	rm -f app *.o
