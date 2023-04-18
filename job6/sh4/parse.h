#ifndef _PARSE_H
#define _PARSE_H

#include "cmd.h"

#define MAX_CMDC 10
#define BUFFER_MAX 65535
extern int parse_pipe_cmd(char *line, struct cmd *cmdv);
extern void dump_pipe_cmd(int cmdc, struct cmd *cmdv);

extern void test_parse_cmd_1();
extern void test_parse_cmd_2();
extern void test_parse_pipe_cmd_1();
extern void test_parse_pipe_cmd_2();

#endif
