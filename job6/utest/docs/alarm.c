#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    alarm(3);

    int i;
    for (i = 0; ; i++) {
        printf("%d\n", i);
        sleep(1);
    }
    return 0;
}
