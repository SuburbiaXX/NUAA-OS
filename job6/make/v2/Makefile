all: main.o min.o max.o
	cc -o app main.o min.o max.o 

main.o: main.c
	cc -c main.c

min.o: min.c
	cc -c min.c

max.o: max.c
	cc -c max.c

clean:
	rm -f app *.o

main.o: main.c min.h max.h
min.o: min.c min.h
max.o: max.c max.h
