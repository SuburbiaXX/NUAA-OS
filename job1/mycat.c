#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int file = -1;
	char *filename = argv[1],buffer[65535];
	ssize_t data = -1;
	file = open(filename, O_RDONLY);
	if(file == -1){
		printf("Can't open file %s.\n", filename);
		return 0;
	}
	data = read(file, buffer, 65535);
	if(data == -1){
		close(file);
		printf("Can't read file %s.\n", filename);
		return 0;
	}
	for(int i = 0;i < data;i++){
		printf("%c", *(buffer + i));
	}
	printf("\n");
	return 0;
}
