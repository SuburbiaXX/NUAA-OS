#ifndef _UTEST_H
#define _UTEST_H

typedef struct {
    int id;
    char *desc;
    void (*fp)();
    int ok;
} utest_t;

extern void utest_add(char *desc, void (*fp)());
extern void utest_run(void);
extern void utest_parse_args(int argc, char *argv[], 
                             char *target_arg, void (*fp)());
extern void utest_exec(utest_t *utest, int utest_log);
extern void utest_report(int ok);

#define UTEST_ADD(fn)       \
    extern void fn();       \
    utest_add(#fn, fn)

#endif
