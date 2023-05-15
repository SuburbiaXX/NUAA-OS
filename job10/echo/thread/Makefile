CC 	:= gcc
CFLAGS  := -O -Wall -Wno-unused -Wno-unused-result -Wno-parentheses -Wundef

all:
	$(CC) $(CFLAGS) -o server server.c -lpthread
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client test

kill:
	sudo pkill -9 httpd
