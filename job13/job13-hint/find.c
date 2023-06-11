char *global_dir;
char *global_name;
coro_t *finder;
coro_t *fs_walker;

void find()
{
    while (1) {
        char *item = (char *) coro_resume(fs_walker);
        if (item == NULL)
            break;
        if (item ends with global_name)
            puts(item);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    get global_dir from arg;
    get global_name from arg;

    finder = coro_new(find);
    fs_walker = coro_new(fs_walk);
    coro_boot(finder);
    return 0;
}
