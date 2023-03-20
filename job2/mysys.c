#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void mysys(char *command){
	pid_t pid;
	pid = fork();
	// 用子进程执行，因为要调用execvp()，会覆盖当前程序
	if(pid == 0){
		char buf[65535];
	    char *p, *argv[65535];
		int i = 0, error = -1;
		strncpy(buf, command, strlen(command)+1);
		// 字符串拆分获得参数
		p = strtok(buf, " ");
		while(p != NULL){
			argv[i ++] = p;
			p = strtok(NULL, " ");
		}
		argv[i] = NULL;

		error = execvp(argv[0], argv);
		if(error == -1){
			perror(argv[0]);
			return;
		}
	}
	// 父进程等待子进程结束
	wait(NULL);
}

int main(){
	printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
