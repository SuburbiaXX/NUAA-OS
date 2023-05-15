#include "echo.h"
#include <pthread.h>

void *client_thread(void *arg) {
    char *ip_address = "127.0.0.1";
    int port = 1234;
    char msg[10] = "hello";
    // 向 msg 字符串最后随机加入 a~z 之间的字符
    char ch = 'a' + rand() % 26;
    msg[5] = ch;
    printf("new_msg: %s\n", msg);
    run_echo_client(ip_address, port, &msg);
    pthread_exit(NULL);
}

void run_echo_client(char *ip_address, int port, char *msg) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        error("socket");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_port = htons(port);

    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
        error("connect");

    char buff[128];
    // printf("Enter a message: ");
    // fgets(buff, sizeof(buff), stdin);
    printf("client send: %s\n", msg);
    write(fd, msg, strlen(msg));

    int count = read(fd, buff, sizeof(buff));
    printf("server response: %.*s\n", count, buff);

    close(fd);
}

int main(int argc, char *argv[])
{
    int num_threads = 10;
    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0)
            error("pthread_create");
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0)
            error("pthread_join");
    }

    return 0;
}
