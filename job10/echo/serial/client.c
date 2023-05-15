#include "echo.h"

int create_client(char *ip_address, int port)
{
    struct sockaddr_in server;
    int fd;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        error("socket");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_port = htons(port);
    if (connect(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0)
        error("bind");
    return fd;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        puts("Usage: client msg\n");
        return 0;
    }
    char *msg = argv[1];

    int client = create_client("127.0.0.1", 1234);

    // send(client, msg, strlen(msg), 0);
    write(client, msg, strlen(msg));
    printf("client send: %s\n", msg);

    // recv(client, buff, sizeof(buff), 0);
    char buff[128];
    int count = read(client, buff, sizeof(buff));
    buff[count] = 0;
    printf("client receive: %s\n", buff);
    return 0;
}
