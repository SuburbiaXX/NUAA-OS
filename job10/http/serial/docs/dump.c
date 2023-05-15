#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envv[])
{
    int i;

    printf("argc = %d\n", argc);
    for (i = 0; i < argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    puts("---------------------------------------------");

    for (i = 0; envv[i] != NULL; i++) {
        char *env = envv[i];
        puts(env);
    }
    return 0;
}
