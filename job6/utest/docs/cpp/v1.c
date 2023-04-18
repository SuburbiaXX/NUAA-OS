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
    test("f1", f1);
    test("f2", f2);
    return 0;
}
