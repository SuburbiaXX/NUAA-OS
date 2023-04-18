#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "cmd.h"
#include "parse.h"
#include "utest.h"

struct cmd command[MAX_CMDC];

void utest_add_and_run() {// 添加 utest 并且测试
    UTEST_ADD(test_exec_cmd);
    UTEST_ADD(test_exec_pipe_cmd);
    UTEST_ADD(test_parse_cmd_1);
    UTEST_ADD(test_parse_cmd_2);
    UTEST_ADD(test_parse_pipe_cmd_1);
    UTEST_ADD(test_parse_pipe_cmd_2);
    utest_run();
}

void read_line(char *line, int size) {// 读入一行
    int cnt;
    cnt = read(0, line, BUFFER_MAX);
    if(cnt > 1) {// 去掉最后的回车
        line[cnt - 1] = 0;
    }
}

void clean(int cmdc, struct cmd *cmdv) {// 释放空间
    for(int i = 0; i < cmdc; i ++ ) {// 对每个管道
        for(int j = 0; j <= cmdv[i].argc; j ++ ) {// 具体某个管道
            if(cmdv[i].argv[j] != NULL) {
                free(cmdv[i].argv[j]);
                cmdv[i].argv[j] = NULL;
            }
        }
        if(cmdv[i].input != NULL){
            free(cmdv[i].input);
            cmdv[i].input = NULL;
        }
        if(cmdv[i].output != NULL){
            free(cmdv[i].output);
            cmdv[i].output = NULL;
        }
    }
}


int main(int argc, char *argv[]) {
    utest_parse_args(argc, argv, "-utest", utest_add_and_run);// 判断是否进入 utest 模式
    int cmdc = 0;
    char buf[BUFFER_MAX] = "";
    while (1) {  
        write(1, "SHELL> ", sizeof("SHELL> "));
        read_line(buf, BUFFER_MAX);
        if(strcmp(buf, "exit") == 0) {// 提前判断 exit, 否则会陷入子进程出不来
            return 0;
        }

        cmdc = parse_pipe_cmd(buf, command);
        // dump_pipe_cmd(cmdc, command);
        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            continue;
        }
        if(pid == 0){
            exec_pipe_cmd(cmdc, command);
        }
        wait(NULL);
        clean(cmdc, command);
    }
    return 0;
}
