// 引用 fs_walk.c
// 编写一个消费者协程，获取 fs_walk 传递的目录项
// 打印符合要求的目录项

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "coro.h"
#include "fs_walk.h"

coro_t *coro_p, *coro_c;
char *name, *dir;

void usage() {
    puts("Usage:");
    puts("find -name file-name dir");
}

void produce() {
    fs_walk(dir);
    exit(0);
}

void consume() {
    while (1) {
        char* file_name = (char*)coro_resume(coro_p);
        if (file_name == NULL) {
            break;
        }
        // 判断是否存在 name (这里偷懒了, 实现的是模糊查找)
        if (strstr(file_name, name) != NULL) {
            printf("FIND %s\n", file_name);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
        exit(1);
    }
    name = argv[2];
    dir = argv[3];

    coro_p = coro_new(produce);
    coro_c = coro_new(consume);

    coro_boot(coro_c);
    return 0;
}
