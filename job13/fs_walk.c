// fs_walk 是一个生产者协程
// 遍历指定目录下所有的目录项
// 使用 co_yield 函数向消费者协程传递目录项

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "coro.h"
#include "fs_walk.h"

void fs_walk(char *dir) {
    if (dir == NULL) {
        coro_yield(0);
    }
    DIR *d = opendir(dir);
    // printf("dir: %s\n", dir);
    if (d == NULL) {
        perror("opendir");
        exit(1);
    }
    struct dirent *de;
    // 递归查找, 如果是文件夹就进入
    while ((de = readdir(d)) != NULL) {
        if (de->d_type == 4) {
            // printf("Nowdir: %s\n", de->d_name);
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
                continue;
            }
            char *subdir = malloc(strlen(dir) + strlen(de->d_name) + 2);
            strcat(strcat(strcpy(subdir, dir), "/"), de->d_name);
            // printf("subdir: %s\n", subdir);
            fs_walk(subdir);
            free(subdir);
        } else {
            // printf("yield: %s\n", de->d_name);            
            // 构造文件路径
            // printf("dir: %s\n", dir);
            char *file_path = malloc(strlen(dir) + strlen(de->d_name) + 2);
            strcat(strcat(strcpy(file_path, dir), "/"), de->d_name);
            coro_yield((intptr_t)file_path);
            free(file_path);
        }
    }
    closedir(d);
}