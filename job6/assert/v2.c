#include <stdio.h>
#include <stdlib.h>

#define assert(expr)                                        \
    if (!(expr)) {                                          \
        fprintf(stderr, "Assertion failed: %s\n", #expr);   \
        abort();                                            \
    }

int main()
{
    assert(1 < 2);
    assert(2 < 1);
    return 0;
}
