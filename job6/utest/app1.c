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

// Time out
int mul(int a, int b)
{
    while (1)
        ;
    return a * b;
}

// Program crashed
int div0(int a, int b)
{
    return a / (a - b);
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

void test_div0()
{
    assert(div0(5, 2) == 2);
}

void test_mod()
{
    assert(mod(5, 2) == 1);
}

void utest_add_and_run()
{
    // fork, in child, exec test_add, wait
    UTEST_ADD(test_add); 

    // fork, in child, exec test_sub, wait
    UTEST_ADD(test_sub);

    // fork, ...
    UTEST_ADD(test_mul);

    // fork, ...
    UTEST_ADD(test_div0);

    // fork, ...
    UTEST_ADD(test_mod);

    utest_run();
}

int main(int argc, char *argv[])
{
    utest_parse_args(argc, argv, "-utest", utest_add_and_run);
    puts("world");
    return 0;
}
