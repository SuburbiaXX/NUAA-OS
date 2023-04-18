#include <stdio.h>
#include "max.h"

void print_max(int a, int b)
{
    if (a > b)
        printf("max = %d\n", a);
    else
        printf("max = %d\n", b);
}
