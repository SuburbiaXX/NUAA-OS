#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void mysys(char *command){
	// 拆分指令及其参数
	char buf[65535];
	char *p, *argv[65535];
	int i = 0, error = -1;
	strncpy(buf, command, strlen(command)+1);
	p = strtok(buf, " ");
	while(p != NULL){
		argv[i ++] = p;
		p = strtok(NULL, " ");
	}
	argv[i] = NULL;

	// 实现 cd / pwd / exit 指令
	if(strcmp(argv[0], "cd") == 0){
		int err = chdir(argv[1]);
		if(err >= 0){
			char *path = getcwd(NULL, 0);
			printf("Now at : %s\n", path);
			free(path);
		}else{
			puts("change dir error!");
		}
		return;
	}else if(strcmp(argv[0], "pwd") == 0){
		char *path = getcwd(NULL, 0);
		printf("%s\n", path);
		free(path);
		return;
	}else if(strcmp(argv[0], "exit") == 0){
		exit(0);
	}


	pid_t pid;
	pid = fork();
	if(pid == 0){
		error = execvp(argv[0], argv);
		if(error == -1){
			perror(argv[0]);
			exit(0);
		}
	}
	wait(NULL);
}
int main(){
    char line[65535];
	int c;
	while(1){
		write(1, "> ", sizeof("> "));
		c = read(0, line, sizeof(line));
		if(c > 1){// 滤去不输入，直接回车
			line[c - 1] = 0;	// 删除最后读入的回车
			mysys(line);
		}
	}
	return 0;
}
