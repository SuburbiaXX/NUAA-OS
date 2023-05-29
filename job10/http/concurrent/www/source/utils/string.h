#ifndef _UTILS_STRING_H
#define _UTILS_STRING_H

extern char *strdup(char *string);
extern char *strcpy(char *to, char *from);
extern char *strncpy(char *to, char *from, int n);
extern char *strcat(char *to, char *from);
extern int strlen(char *string);
extern char *strchr(char *string, int c);
extern char *strrchr(char *string, int c);
extern int strcmp(char *from, char *to);
extern void *memmove(void *to, void * from, size_t count);
extern void *memset(void *pointer, int value, size_t count);
extern void *memcpy(void *to, void *from, size_t count);
extern int split_string(char *string, char *seperator, char *word_table[]);

#endif
