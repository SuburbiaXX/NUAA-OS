#include "fs_walk.h"

void find_dir(char *dir_path)
{
    DIR *dir = open(dir_path);
    while (entry = readdir(dir)) {
        if (entry is dir)
            find_dir(dir_path + "/" + entry.d_name);

        if (entry is regualar)
            coro_yield((char *) entry.d_name);
    }
    closedir(dir);
}

void fs_walk()
{
    find_dir(global_dir_path);
    coro_yield(NULL);
}

