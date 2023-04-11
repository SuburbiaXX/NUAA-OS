run: sci.o main.o
		cc -o app sci.o main.o

main.o: main.c sci.h
		cc -c main.c

sci.o: sci.c sci.h
		cc -c sci.c

clean: 
		rm -f app *.o

