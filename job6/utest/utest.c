#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include "utest.h"


static int utest_capacity;
static utest_t *utest_array;
static int utest_count;
static int utest_log;

void utest_add(char *desc, void (*fp)()) {
    assert(utest_count <= utest_capacity);

    if (utest_capacity == 0) {
        utest_capacity = 2;
        utest_array = malloc(utest_capacity * sizeof(utest_t));
    }

    if (utest_count == utest_capacity) {
        utest_capacity *= 2;
        utest_array = realloc(utest_array, utest_capacity * sizeof(utest_t));
    }

    utest_t *utest = utest_array + utest_count;
    utest->id = utest_count;
    utest->desc = strdup(desc);
    utest->fp = fp;
    utest_count++;
}

void utest_parse_args(int argc, char *argv[], char *target_arg, void (*fp)()) {
    for (int i = 0; i < argc; i++) {
        char *arg = argv[i];
        if (strcmp(arg, target_arg) == 0) {
            fp();
            exit(EXIT_SUCCESS);
        }
    }
}

void utest_report(int ok) {
    if(ok) {
        puts("Pass all the test.");
    } else {
        puts("Fail some of the test.");
        for(int i = 0; i < utest_count; i++) {
            if(utest_array[i].ok == 0) {
                printf("%03d: %-32s\n", utest_array[i].id, utest_array[i].desc);
            }
        }
    }
    putchar(10);
    puts("See utest.log for details.");
    return;
}

void utest_exec(utest_t *utest, int utest_log) {
    printf("%03d: %-32s", utest->id, utest->desc);
    fflush(stdout);
    dup2(utest_log, 1);// 重定向标准输出和标准错误到utest.log
    dup2(utest_log, 2);
    close(utest_log);
    utest->fp();
    exit(0);
}

void utest_run(void) {
    int all_ok = 1, utest_log, status;
    pid_t pid;

    utest_log = open("utest.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (utest_log < 0) {
        perror("open");
        puts("open utest.log failed");
        exit(1);
    }

    for (int i = 0; i < utest_count; i++) {
        pid = fork();
        if (pid == 0) {
            alarm(3);
            utest_exec(&utest_array[i], utest_log);
            // utest_array[i].fp();
            // exit(0);
        }
        utest_array[i].ok = waitpid(pid, &status, 0) && WIFEXITED(status) ? 1 : 0;
        if(utest_array[i].ok == 0) {
            all_ok = 0;
        }
        printf("\t\t%s\n", utest_array[i].ok ? "\033[32m\u2714\033[0m" : "\033[31m\u2718\033[0m");
    }
    putchar(10);
    utest_report(all_ok);
    close(utest_log);
    return;
}
