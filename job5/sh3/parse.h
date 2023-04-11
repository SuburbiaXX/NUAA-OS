#ifndef _PARSE_H
#define _PARSE_H

#include "cmd.h"

#define MAX_CMDC 10
#define BUFFER_MAX 65535
extern int parse_pipe_cmd(char *line, struct cmd *cmdv);
extern void dump_pipe_cmd(int cmdc, struct cmd *cmdv);

#endif
