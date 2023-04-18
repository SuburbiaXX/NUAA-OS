#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utest.h"

int add(int a, int b)
{
    return a + b;
}

// Logical error
int sub(int a, int b)
{
    return a * b;
}

int mul(int a, int b)
{
    return a * b;
}

int mod(int a, int b)
{
    return a % b;
}

void test_add()
{
    assert(add(1, 1) == 2);
}

void test_sub()
{
    assert(sub(1, 1) == 0);
}

void test_mul()
{
    assert(mul(1, 1) == 1);
}

void test_mod()
{
    assert(mod(5, 2) == 1);
}

#define UTEST_ADD(fn)       \
    extern void fn();       \
    utest_add(#fn, fn)

void utest_add_and_run()
{
    UTEST_ADD(test_add);
    UTEST_ADD(test_sub);
    UTEST_ADD(test_mul);
    UTEST_ADD(test_mod);
    utest_run();
}

int main(int argc, char *argv[])
{
    utest_parse_args(argc, argv, "-utest", utest_add_and_run);
    puts("hello");
    return 0;
}
