#include <stdio.h>
#include "min.h"

void print_min(int a, int b)
{
    if (a < b)
        printf("min = %d\n", a);
    else
        printf("min = %d\n", b);
}
