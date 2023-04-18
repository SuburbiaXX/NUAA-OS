#define TEST(fn)            \
    test(#fn, fn)

void f1()
{
    puts("Exec f1");
}

void f2()
{
    puts("Exec f2");
}

typedef void (*fp_t)();

void test(char *fn_name, fp_t fp)
{
    printf("Test %s\n", fn_name);
    fp();
    printf("Test End\n\n");
}

int main()
{
    TEST(f1);
    TEST(f2);
    return 0;
}
