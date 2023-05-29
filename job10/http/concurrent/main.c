#include "std.h"
#include "tcp.h"
#include "http.h"

void usage() {
    puts("Usage: httpd -p port -h");
    puts("  -p port");
}

int main(int argc, char *argv[]) {
    char *ip_address = "0.0.0.0";
    int port = 80;// 默认 80 端口

    for (int i = 1; i < argc; i++) {// 根据传入参数调整
        if (strcmp(argv[i], "-p") == 0) {
            i++;
            port = atoi(argv[i]);
            continue;
        }
    }

    int server_fd = tcp_listen(ip_address, port);// 创建监听
    while (1) {
        int client_fd = tcp_accept(server_fd);// 接受连接
        if (client_fd < 0)
            panic("tcp_accept");
    
        http_handler(client_fd);// 处理请求
        close(client_fd);// 关闭连接
    }

    return 0;
}
