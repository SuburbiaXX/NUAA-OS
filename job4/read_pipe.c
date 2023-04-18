#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


void read_pipe(char *command) {
    pid_t pid;
    char buf[65535],temp[65535];
    char *p, *argv[65535];
    int i = 0, error = -1;
    strncpy(temp, command, strlen(command)+1);
    p = strtok(temp, " ");
    while(p != NULL){
        argv[i ++ ] = p;
        p = strtok(NULL, " ");
    }
    argv[i] = NULL;

    int fd[2];
    pipe(fd);
    pid = fork();
    if(pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);

		error = execvp(argv[0], argv);
		if(error == -1){
			perror(argv[0]);
			return;
		}
    }
    // 或者直接
    // read(fd[0], buf, 65535)
    dup2(fd[0], 0);
    close(fd[0]);
    close(fd[1]);
	while(read(0, buf, 65535)) {
		write(1, buf, sizeof(buf));
	}
}

int main() {
    printf("--------------------------------------------------\n");
    read_pipe("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    read_pipe("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
