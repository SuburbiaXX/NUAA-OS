#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char *argv[] = {"./dump", "hello", "world", NULL};
    char *envv[] = {"NAME=tom", "AGE=100", "gender=male", NULL};

    execve(argv[0], argv, envv);
    perror("execve"); 
    return 0;
}
