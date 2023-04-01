#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void copy(int src_fd, int dst_fd){
    char buf[512];
    int count;
    while ((count = read(src_fd, buf, 512))){
        write(dst_fd, buf,count);
    }
}



int main(int argc, char *argv[]){
    int fd;
    if (argc == 1) {
        fd = 0;
    }else {
        fd = open(argv[1], O_RDONLY);
    }
    copy(fd, 1);
}
