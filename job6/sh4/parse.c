#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "parse.h"

void parse_cmd(char *line, struct cmd *cmd) {// 拆分某一管道, 注意空间申请和释放
    char *p = NULL, buffer[BUFFER_MAX] = "";
    cmd->argc = 0;
    strncpy(buffer, line, strlen(line) + 1);
    p = strtok(buffer, " ");
    while(p != NULL) {
        if(strchr(p, '<')) {// 输入重定向
            cmd->input = (char *)malloc(sizeof(char) * (strlen(p) + 1));
            strncpy(cmd->input, p + 1, strlen(p));
        }
        else if(strchr(p, '>')) {// 输出重定向
            cmd->output = (char *)malloc(sizeof(char) * (strlen(p) + 1));
            strncpy(cmd->output, p + 1, strlen(p));
        }
        else {// 参数
            cmd->argv[cmd->argc] = (char *)malloc(sizeof(char) * (strlen(p) + 1));
            strncpy(cmd->argv[cmd->argc], p, strlen(p) + 1);
            cmd->argc ++;
        }
        p = strtok(NULL, " ");
    }
    cmd->argv[cmd->argc] = NULL;
    return;
}

void dump_cmd(struct cmd *cmd) {// 调试 -> 输出某一管道中命令
    int i;
    printf("argv[0]%s\n", cmd->argv[0]);
    for (i = 1; i < cmd->argc; i++) {
        printf("%s/", cmd->argv[i]);
    }
    printf("\nINPUT:%s\n", cmd->input ? cmd->input : "NULL");
    printf("OUTPUT:%s", cmd->output ? cmd->output : "NULL");
    printf("\n\n");
}

int parse_pipe_cmd(char *line, struct cmd *cmdv) {// 拆分统计管道命令个数
    int cmdc = 0;
    char buffer[BUFFER_MAX] = "", *p = NULL, *save = NULL;
    strncpy(buffer, line, strlen(line) + 1);
    p = strtok_r(buffer, "|", &save);  // strtok_r()保证字符串能被正常分割
    while(p != NULL) {// 细化至具体某一管道
        parse_cmd(p, &cmdv[cmdc]);
        p = strtok_r(NULL, "|", &save);
        cmdc ++;
    }
    return cmdc;
}

void dump_pipe_cmd(int cmdc, struct cmd *cmdv) {// 调试 -> 输出每个管道命令
    int i;
    printf("pipe cmd, cmdc = %d\n", cmdc);
    for (i = 0; i < cmdc; i++) {
        printf("cmd %d:\n", i);
        struct cmd *cmd = &cmdv[i];
        dump_cmd(cmd);
    }
}


// echo abc xyz
void test_parse_cmd_1() {// 测试拆分某一指令的结果
    struct cmd cmd;
    char line[] = "echo abc xyz";
    parse_cmd(line, &cmd);

    assert(cmd.argc == 3);
    assert(strcmp(cmd.argv[0], "echo") == 0);
    assert(strcmp(cmd.argv[1], "abc") == 0);
    assert(strcmp(cmd.argv[2], "xyz") == 0);
    assert(cmd.argv[3] == NULL);
}

// echo abc >log
void test_parse_cmd_2() {// 测试拆分某一指令的结果, 包括重定向
    struct cmd cmd;
    char line[] = "echo abc >log";
    parse_cmd(line, &cmd);

    assert(cmd.argc == 2);
    assert(strcmp(cmd.argv[0], "echo") == 0);
    assert(strcmp(cmd.argv[1], "abc") == 0);
    assert(cmd.argv[2] == NULL);
    assert(strcmp(cmd.output, "log") == 0);
}

// cat /etc/passwd | wc -l
void test_parse_pipe_cmd_1() {// 测试拆分管道命令的结果
    struct cmd cmdv[2];
    int cmdc;
    char line[] = "cat /etc/passwd | wc -l";
    cmdc = parse_pipe_cmd(line, cmdv);

    assert(cmdc == 2);
    assert(strcmp(cmdv[0].argv[0], "cat") == 0);
    assert(strcmp(cmdv[0].argv[1], "/etc/passwd") == 0);
    assert(cmdv[0].argv[2] == NULL);
    assert(strcmp(cmdv[1].argv[0], "wc") == 0);
    assert(strcmp(cmdv[1].argv[1], "-l") == 0);
    assert(cmdv[1].argv[2] == NULL);
}

// cat <input | sort | cat >output
void test_parse_pipe_cmd_2() {// 测试拆分管道命令的结果, 包括重定向
    struct cmd cmdv[3];
    int cmdc;
    char line[] = "cat <input | sort | cat >output";
    cmdc = parse_pipe_cmd(line, cmdv);

    assert(cmdc == 3);
    assert(strcmp(cmdv[0].argv[0], "cat") == 0);
    assert(cmdv[0].argv[1] == NULL);
    assert(strcmp(cmdv[0].input, "input") == 0);
    assert(strcmp(cmdv[1].argv[0], "sort") == 0);
    assert(cmdv[1].argv[1] == NULL);
    assert(strcmp(cmdv[2].argv[0], "cat") == 0);
    assert(cmdv[2].argv[1] == NULL);
    assert(strcmp(cmdv[2].output, "output") == 0);
}
