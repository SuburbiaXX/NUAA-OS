#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envv[])
{
    printf("<h1>Enviroment variables</h1>\n");
    printf("<ul>\n");

    int i;
    for (i = 0; envv[i] != NULL; i++) {
        char *env = envv[i];
        printf("<li>%s</li>\n", env);
    }

    printf("</ul>\n");
    return 0;
}
