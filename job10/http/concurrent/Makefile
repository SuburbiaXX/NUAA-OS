CC 	:= gcc
CFLAGS  := -g -fsanitize=address -O -Wall -Wno-unused -Wno-unused-result -Wno-parentheses -Wundef

all:
	$(CC) $(CFLAGS) -o httpd main.c http.c tcp.c

run:
	./httpd -d -p 8080

clean:
	rm -f httpd tags

kill:
	sudo pkill -9 httpd
