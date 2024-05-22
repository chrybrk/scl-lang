#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "include/utils.h"

char* writef(char* s, ...)
{
    char* string;
    va_list args;
    va_start(args, s);
    vasprintf(&string, s, args);
    va_end(args);
    return string;
}

char* alloc_str(char* src, char* dst)
{
    if (dst == NULL)
    {
        dst = calloc(1, sizeof(char) * strlen(src) + 1);
        dst = strcpy(dst, src);
        return dst;
    }

    dst = realloc(dst, sizeof(char) * (strlen(dst) + strlen(src) + 2));
    dst = strcat(dst, src);
    return dst;
}
