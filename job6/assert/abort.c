#include <stdio.h>
#include <stdlib.h>

int main()
{
    puts("Before abort");
    abort();
    puts("After abort");
    return 0;
}
