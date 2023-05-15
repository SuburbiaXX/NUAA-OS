#include "std.h"
#include "tcp.h"

void http_read_req(int fd)
{
    FILE *fr = fdopen(fd, "r");

    char line[100];
    fgets(line, 100, fr);
    printf("%s", line);
}

void http_handler_1(int fd)
{
    FILE *fw = fdopen(fd, "w");

    char *content = "<h1>Hello</h1><h1>World</h1>";
    int content_length = strlen(content);

    fprintf(fw, "HTTP/1.0 200 OK\r\n");
    fprintf(fw, "Server: tiny httpd\r\n");
    fprintf(fw, "Content-type: text/html\r\n");
    fprintf(fw, "Content-length: %d\r\n", content_length);
    fprintf(fw, "Connection: close\r\n");
    fprintf(fw, "\r\n");

    fwrite(content, content_length, 1, fw);
    fflush(fw);
}

void http_write_chunk(FILE *fw, void *chunk, int size)
{
    fprintf(fw, "%x\r\n", size);
    fwrite(chunk, size, 1, fw);
    fprintf(fw, "\r\n");
}

void http_handler_2(int fd)
{
    FILE *fw = fdopen(fd, "r+");

    fprintf(fw, "HTTP/1.1 200 OK\r\n");
    fprintf(fw, "Server: tiny httpd\r\n");
    fprintf(fw, "Content-type: text/html\r\n");
    fprintf(fw, "Transfer-Encoding: chunked\r\n");
    fprintf(fw, "\r\n");

    char *hello = "<h1>HELLO</h1>";
    http_write_chunk(fw, hello, strlen(hello));

    char *world = "<h1>WORLD</h1>";
    http_write_chunk(fw, world, strlen(world));

    char *zero = "";
    http_write_chunk(fw, zero, strlen(zero));

    fflush(fw);
}

int main(int argc, char *argv[])
{
    char *ip_address = "0.0.0.0";
    int port = 8080;

    int server_fd = tcp_listen(ip_address, port);
    while (1) {
        int client_fd = tcp_accept(server_fd);
        http_read_req(client_fd);
        http_handler_1(client_fd);
        close(client_fd);
    }
    return 0;
}
