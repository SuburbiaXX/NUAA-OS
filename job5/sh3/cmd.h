#ifndef _CMD_H
#define _CMD_H

// "echo abc >log"
// argc = 2
// argv = {"echo", "abc"}
// input = NULL
// output = "log"

#define MAX_ARGC 10
struct cmd {
    int argc;
    char *argv[MAX_ARGC];
    char *input;
    char *output;
};


extern void exec_cmd(struct cmd *cmd);
extern int builtin_cmd(struct cmd *cmd);
extern void exec_pipe_cmd(int cmdc, struct cmd *cmdv);

#endif
