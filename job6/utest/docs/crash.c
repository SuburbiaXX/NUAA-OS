#include <stdio.h>

int main()
{
    int *p = NULL;

    puts("Before dangerous opertaion");
    *p = 123;
    puts("After dangerous opertaion");
    return 0;
}
