#include <stdio.h>
#include <stdlib.h>
#include "coro.h"

coro_t * coro_p, * coro_f, * coro_c;

#define N 8

void produce() {
    int i;

    for (i = 0; i < N; i++) {
        printf("produce %d\n", i);
        int x = coro_resume(coro_f);
        coro_yield(x);
    }
    puts("\t-- NEVER REACH HERE --");
}

void filter() {
    int i;

    for (i = 0; i < N; i++) {
        printf("\tfilter %d -> %d\n", i, i * 10);
        coro_yield(i * 10);
    }
    puts("\t-- NEVER REACH HERE --");
}

void consume() {
    int i;

    puts("\t\t-- CONSUME --");
    puts("\t-- FILTER --");
    puts("-- PRODUCE --");
    for (i = 0; i < N; i++) {
        int x = coro_resume(coro_p);
        printf("\t\tconsume %d\n", x);
    }
    puts("\t\t-- END --");
    exit(0);
}

int main() {
    coro_p = coro_new(produce);
    coro_f = coro_new(filter);
    coro_c = coro_new(consume);
    coro_boot(coro_c);
    return 0;
}
