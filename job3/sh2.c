#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void mysys(char *command){
	// 拆分指令及其参数
	char buf[65535];
	char *p, *argv[65535];
	int count = 0, error = -1;
	strncpy(buf, command, strlen(command) + 1);
	p = strtok(buf, " ");
	while(p != NULL){
		argv[count ++] = p;
		p = strtok(NULL, " ");
	}
	argv[count] = NULL;
    
    // for (int j = 0; j < count; j ++) {
    //     printf("%s\n", argv[j]);
    // }


	// 实现 cd / pwd / exit 指令
	if(strcmp(argv[0], "cd") == 0){
		int err = chdir(argv[1]);
		if(err >= 0){
			char *path = getcwd(NULL, 0);
			printf("Now at : %s\n", path);
			free(path);
		}else{
			// puts("change dir error!");
            perror(argv[0]);
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

    // for (int j = 0; j < count; j ++) {
    //     printf("%s\n", argv[j]);
    // }


	pid_t pid;
	pid = fork();
	if(pid == 0){
		// 处理重定向的符号
		int i;
		// printf("count is %d\n", count);
		int in_fd = -1, out_fd = -1;
		for (i = 1; i < count; i ++) {
			char temp[65535], temp1[65535];
			// printf("now i = %d\n", i);
			strcpy(temp, argv[i]);
			// printf("%s\n", temp);
			if(temp[0] - '<' == 0) {// 重定向输入
				// 删除重定向符号
				// printf("Here-1\n");
				strncpy(temp1, temp + 1, strlen(temp));
				strncpy(argv[i], temp1, strlen(temp1) + 1);
				//printf("%s\n", argv[i]);
				in_fd = open(argv[i], O_RDONLY);
				if (in_fd == -1) {
					perror(argv[i]);
					exit(0);
				}
				dup2(in_fd, 0); // 标准输入定向到此文件
				close(in_fd);   // 关闭输入文件
				argv[i] = NULL; // 文件名置空, 避免继续传递
				// printf("删除 < \n");
			}else if(temp[0] - '>' == 0 && temp[1] - '>' != 0) {// 重定向输出
				// printf("Here-2\n");
				// printf("temp = %s\n", temp);
				// printf("temp1 = %s\n", temp1);
				strncpy(temp1, temp + 1, strlen(temp));
				// printf("temp1 = %s\n", temp1);
				// strcpy(argv[i], "");
				// printf("argv[i] = %s\n", argv[i]);
				strncpy(argv[i], temp1, strlen(temp1) + 1);
				// printf("argv[i] = %s\n", argv[i]);
				// printf("%d\n", strlen(argv[i]));
				// 打开输出的文件, 如果不存在则创建, 存在则清空
				out_fd = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, 0755);
				if(out_fd == -1) {
					perror(argv[i]);
					exit(0);
				}
				dup2(out_fd, 1);    // 标准输出重定向
				close(out_fd);      // 关闭输出文件
				argv[i] = NULL;     // 文件名置空, 避免继续传递

			}else if(temp[0] - '>' == 0 && temp[1] - '>' == 0) {// 追加输出
				// printf("Here-3\n");
				strncpy(temp1, temp + 2, strlen(temp) - 2);
				strcpy(argv[i], temp1);
				// 打开输出文件，如果不存在则创建，如果存在则追加
				out_fd = open(argv[i], O_WRONLY | O_CREAT | O_APPEND, 0755);
				if(out_fd == -1) {
					perror(argv[i]);
					exit(0);
				}
				dup2(out_fd, 1);
				close(out_fd);  
				argv[i] = NULL;
			}
		}

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

