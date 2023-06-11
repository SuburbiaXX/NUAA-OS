#include <stdio.h>
#include <stdlib.h>
#include "coro.h"

#define N 4

coro_t *coro_p;
coro_t *coro_c;

void produce()
{
    int i;

    puts("\t-- PRODUCE --");
    for (i = 0; i < N; i++) {
        int x = i * 10;
        printf("\tproduce %d\n", x);
        coro_yield(x);
    }
    puts("\t-- NEVER REACH HERE --");
}

void consume()
{
    int i;

    puts("-- CONSUME --");
    for (i = 0; i < N; i++) {
        int x = coro_resume(coro_p);
        printf("consume %d\n", x);
    }
    puts("-- END --");
    exit(0);
}

int main()
{
    coro_p = coro_new(produce);
    coro_c = coro_new(consume);
    coro_boot(coro_c);
    return 0;
}
