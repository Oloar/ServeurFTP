#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "parse.h"

static void memory_error(void)
{
    errno = ENOMEM;
    perror(0);
    exit(1);
}


static void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (!p) memory_error();
    return p;
}


static void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (!p) memory_error();
    return p;
}

/* Split the string in words, according to the simple shell grammar. */
char **split_in_words(char *line) {
    char *cur = line;
    char **tab = 0;
    size_t l = 0;
    char c;

    while ((c = *cur) != 0) {
        char *w = 0;
        char *start;
        switch (c) {
        case ' ':
        case '\t':
        case '\n':
            /* Ignore any whitespace */
            cur++;
            break;
        default:
            /* Another word */
            start = cur;
            while (c) {
                c = *++cur;
                switch (c) {
                case 0:
                case ' ':
                case '\t':
                case '\n':
                    c = 0;
                    break;
                default: ;
                }
            }
            w = xmalloc((cur - start + 1) * sizeof(char));
            strncpy(w, start, cur - start);
            w[cur - start] = 0;
        }
        if (w) {
            tab = xrealloc(tab, (l + 1) * sizeof(char *));
            tab[l++] = w;
        }
    }
    tab = xrealloc(tab, (l + 1) * sizeof(char *));
    tab[l++] = 0;
    return tab;
}
int nb_words(char **t) {
    int i = 0;
    while(t[i] != 0) {
        i++;
    }
    return i;
}

int command_code(char *action) {
    char *actionStrings[] = {"get","put","quit"};

    for (int i = 0; i < NBACTIONS; ++i) {
        if (strcmp(action, actionStrings[i]) == 0) {
            return i;
        }
    }
    return -1;
}