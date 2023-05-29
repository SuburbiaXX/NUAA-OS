#include <utils/root.h>
#include "string.h"

char *strdup(char *string)
{
    return NULL;
}

char *strcpy(char *to, char *from)
{
    char *result = to;
    while (*to++ = *from++)
        ;
    return result;
}

char *strncpy(char *to, char *from, int n)
{
    int i;

    for (i = 0; i < n && from[i] != '\0'; i++)
        to[i] = from[i];
    for ( ; i < n; i++)
        to[i] = '\0';
    return to;
}

char *strcat(char *to, char * from)
{
    char *result = to;
    while (*to++)
        ;
    to--;
    while (*to++ = *from++)
        ;
    return result; 
}

int strlen(char *s)
{
    char *t = s;
    while (*t++)
        ;
    return t - 1 - s;
}

char *strchr(char *s, int c)
{
    for (; *s; s++)
        if (*s == c)
            return (char*)s;
    return NULL;
}

char *strrchr(char *s, int c)
{
    char *r = (char*)s;
    while (*r++)
        ;
    r -= 2;
    for (; r >= s; r--)
        if (*r == c)
            return r;
    return NULL;
}

int strcmp(char *from, char *to)
{
    while (*from && *to && *from == *to)
        from++, to++;
    return *from - *to;
}

void *memmove(void *to, void *from, size_t count)
{
    void *result = to;
    char *d = (char*) to;
    char *s = (char*) from;

    if (d <= s) {
        while (count--)
            *d++ = *s++;
        return result;
    }
    d += count;
    s += count;
    while (count--)
        *--d = *--s; 
    return result;
}

void *memset(void *pointer, int value, size_t count)
{
    while (count--) {
        *(char*)pointer = value;
        pointer++;
    }
    return pointer;
}

void *memcpy(void *to, void *from, size_t count)
{
    while (count--) {
        *(char*)to = *(char*)from;
        to++;
        from++;
    }
    return to;
}

static char *skip_zero(char *p, char *bound)
{
    if (p == 0)
        return NULL;
    while (*p == 0)
        p++;
    if (p >= bound)
        return NULL;
    return p;
}    

static char *skip_char(char *p, char *bound)
{
    while (*p != 0)
        p++;
    if (p >= bound)
        return NULL;
    return p;
} 

char *strtok(char *string, char *seperator)
{
    static char *p;
    static char *bound;

    if (string) {
        p = string;
        while (*p) {
            if (strchr(seperator, *p))
                *p = 0;
            p++;
        }
        bound = p;
        p = skip_zero(string, bound);
        return p;
    }

    p = skip_char(p, bound);
    p = skip_zero(p, bound);
    return p;
}

/* input string will be destroyed */
int split_string(char *string, char *seperator, char *word_table[])
{
    char *word;
    int word_count = 0;

    word = strtok(string, seperator);
    while (word) {
        word_table[word_count++] = word;
        word = strtok(NULL, seperator);
    }
    return word_count;
}
