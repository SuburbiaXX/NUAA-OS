#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void exec(char *cmd)
{
    printf("Exec %s\n", cmd);

    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    int status;
    int error = waitpid(pid, &status, 0);
    if ((error > 0) && WIFEXITED(status) && (WEXITSTATUS(status) == 0))
        printf("Exec Ok\n");
    else
        printf("Exec Failed\n");

    printf("\n");
}

int main()
{
    exec("./ret0");
    exec("./ret1");
    exec("./exit0");
    exec("./exit0");
    exec("./crash");
    exec("./alarm");
    return 0;
}
