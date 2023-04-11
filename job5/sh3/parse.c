#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "parse.h"

void parse_cmd(char *line, struct cmd *cmd) {// 拆分某一管道, 注意空间申请和释放
    char *p = NULL, buffer[BUFFER_MAX] = "";
    cmd->argc = 0;
    for(int i = 0; i < MAX_ARGC; i ++ ) {
        free(cmd->argv[i]);
        cmd->argv[i] = NULL;
    }
    free(cmd->input);
    free(cmd->output);
    cmd->input = NULL;
    cmd->output = NULL;
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
    return;
}

void dump_cmd(struct cmd *cmd) {// 调试——输出某一管道中命令
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

void dump_pipe_cmd(int cmdc, struct cmd *cmdv) {// 调试——输出每个管道命令
    int i;
    printf("pipe cmd, cmdc = %d\n", cmdc);
    for (i = 0; i < cmdc; i++) {
        printf("cmd %d:\n", i);
        struct cmd *cmd = &cmdv[i];
        dump_cmd(cmd);
    }
}
