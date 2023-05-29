#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envv[])
{
    int i;
    for (i = 0; envv[i] != NULL; i++) {
        char *env = envv[i];
        puts(env);
    }

    puts("---------------------------------------------------");
    char *home;

    home = getenv("HOME");
    printf("HOME=%s\n", home);

    putenv("HOME=/home/nuaa");

    home = getenv("HOME");
    printf("HOME=%s\n", home);
    return 0;
}
