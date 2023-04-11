all: main.o min.o max.o root.o
	cc -o app main.o min.o max.o 

main.o:
	cc -c main.c

min.o:
	cc -c min.c

max.o:
	cc -c max.c

root.o:
	cc -c root.c

clean:
	rm -f app *.o

main.o: main.c min.h max.h root.h
min.o: min.c min.h root.h
max.o: max.c max.h root.h
root.o: root.c root.h
