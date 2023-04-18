#include <stdio.h>
#include <stdlib.h>

int main()
{
    if (!(1 < 2)) {
        fprintf(stderr, "Assertion failed: %s\n", "1 < 2");
        abort();
    }

    if (!(2 < 1)) {
        fprintf(stderr, "Assertion failed: %s\n", "2 < 1");
        abort();
    }
    return 0;
}
