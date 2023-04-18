#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "cmd.h"

void exec_cmd(struct cmd *cmd) {// 执行具体指令
    if(strcmp(cmd->argv[0], "cd") == 0 || strcmp(cmd->argv[0], "pwd") == 0){// 判断是否是内置命令
        builtin_cmd(cmd);
        return;
    }
    int error = -1, fd;
    char temp[256];
    if(cmd->input) {// 存在输入重定向
        fd = open(cmd->input, O_RDONLY);
        if(fd == -1) {
            perror("open");
            return;
        }
        dup2(fd, 0);
        close(fd);
    }
    if(cmd->output) {// 存在输出重定向
        if(cmd->output[0] == '>') {// 考虑追加
            strncpy(temp, cmd->output + 1, strlen(cmd->output));
            fd = open(temp, O_WRONLY | O_CREAT | O_APPEND, 0755);
            if(fd == -1) {
                perror("open");
                return;
            }
            dup2(fd, 1);
            close(fd);
        }else {
            fd = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0755);
            if(fd == -1) {
                perror("open");
                return;
            }
            dup2(fd, 1);
            close(fd);
        }
    }
    error = execvp(cmd->argv[0], cmd->argv);
    if(error == -1){
        perror(cmd->argv[0]);
        return;
    }
}

int builtin_cmd(struct cmd *cmd) {// 执行内置命令 cd / pwd, exit已经在main()中第一步判断
    if(strcmp(cmd->argv[0], "cd") == 0) {
        int error = chdir(cmd->argv[1]);
        if(error >= 0) {
            char *path = getcwd(NULL, 0);
            printf("Now at: %s\n", path);
            free(path);
        }else {
            perror(cmd->argv[0]);
        }
    }else{// pwd
        char *path = getcwd(NULL, 0);
        printf("Now at: %s\n", path);
        free(path);
        return 0;
    }
    return 0;
}

void exec_pipe_cmd(int cmdc, struct cmd *cmdv) {// 管道指令递归实现
    if (cmdc == 1) {// 递归结束
        exec_cmd(cmdv);
        return;
    }

    pid_t pid;
    int fd[2];
    pipe(fd);
    pid = fork();
    if(pid == -1){
        perror("fork");
        return;
    }
    if (pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        exec_pipe_cmd(cmdc - 1, cmdv);
    }
    dup2(fd[0], 0);
    close(fd[0]);
    close(fd[1]);
    exec_cmd(&cmdv[cmdc - 1]);
}

void read_and_cmp(char *path, char *expected) {// 读取对应文件并比较结果是否正确
    int fd = open(path, O_RDONLY);

    char buff[1024];
    int count = read(fd, buff, sizeof(buff));
    buff[count] = 0;

    close(fd);
    assert(strcmp(buff, expected) == 0);
}

// echo hello >test.out
void test_exec_cmd() {// 测试无管道指令
    unlink("test.out");

    pid_t pid = fork();
    if (pid == 0) { 
        struct cmd cmd = {// 构造指令
            2, {"echo", "hello", NULL}, NULL, "test.out"
        };
        exec_cmd(&cmd);
    }
    wait(NULL);

    read_and_cmp("test.out", "hello\n");
}

// cat <test.in | sort | uniq | cat >test.out
// echo "3\n1\n2\n3\n2" | cat >log.txt
void test_exec_pipe_cmd() {// 测试管道指令
    unlink("test.out");

    pid_t pid = fork();
    if (pid == 0) {
        // cat <test.in | sort | uniq | cat >test.out
        struct cmd cmdv[] = {// 构造指令
            {2, {"cat", NULL}, "test.in", NULL},
            {2, {"sort", NULL}, NULL, NULL},
            {2, {"uniq", NULL}, NULL, NULL},
            {2, {"cat", NULL}, NULL, "test.out"},
        };
        exec_pipe_cmd(4, cmdv);
    }
    wait(NULL);

    read_and_cmp("test.out", "1\n2\n3\n");
}
