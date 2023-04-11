#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "cmd.h"
#include "parse.h"

struct cmd command[MAX_CMDC];

void read_line(char *line, int size) {// 读入一行
    int cnt;
    cnt = read(0, line, BUFFER_MAX);
    if(cnt > 1) {// 去掉最后的回车
        line[cnt - 1] = 0;
    }
}

int main() {
    int cmdc = 0;
    char buf[BUFFER_MAX] = "";
    while (1) {  
        write(1, "SHELL> ", sizeof("SHELL> "));
        read_line(buf, BUFFER_MAX);
        if(strcmp(buf, "exit") == 0) {// 提前判断exit, 否则会陷入子进程出不来
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
    }
    return 0;
}
