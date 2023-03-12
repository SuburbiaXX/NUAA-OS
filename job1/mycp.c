#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int file1 = -1,file2 = -1;
	char *source = argv[1], *target = argv[2], buffer[65535];
	ssize_t data = -1, status = -1;
	file1 = open(source, O_RDONLY);
	if(file1 == -1){
		printf("Can't open file %s.\n", source);
		return 0;
	}
	file2 = open(target, O_WRONLY | O_CREAT, 0755);
	if(file2 == -1){
		printf("Can't open file %s.\n", target);
		return 0;
	}
	data = read(file1, buffer, 65535);
	if(data == -1){
		close(file1);
		close(file2);
		printf("Can't read file %s.\n", source);
		return 0;
	}
	status = write(file2, buffer, data);
	if(status == -1){
		close(file1);
		close(file2);
		printf("Can't write to target file %s.\n", target);
		return 0;
	}
	return 0;
}
