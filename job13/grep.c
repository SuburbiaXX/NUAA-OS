// 引用 fs_walk.c 
// 编写一个消费者协程，获取 fs_walk 传递的目录项
// 检索指定的字符串 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coro.h"
#include "fs_walk.h"

char *string, *dir;
coro_t *coro_p, *coro_c;

void usage() {
    puts("Usage:");
    puts("grep -r string dir");
}

void produce() {
    fs_walk(dir);
    exit(0);
}

void consume() {
    while (1) {
        char *file_name = (char *)coro_resume(coro_p);
        if (file_name == NULL) {
            break;
        }
        // 读取文件判断是否存在 string
        FILE *f = fopen(file_name, "r");
        if (f == NULL) {
            perror("fopen");
            exit(1);
        }
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, f) != -1) {
            if (strstr(line, string) != NULL) {
                printf("FIND %s: %s", file_name, line);
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
        exit(1);
    }

    string = argv[2];
    dir = argv[3];

    coro_p = coro_new(produce);
    coro_c = coro_new(consume);
    coro_boot(coro_c);

    return 0;
}
