#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
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

char* remove_whitespace(char* src)
{
    char* new_string = NULL;

    for (size_t i = 0; i < strlen(src); i++)
    {
        if (isalnum(src[i]) || ispunct(src[i]) || src[i] == ' ')
        {
            char s[2] = { src[i], '\0' };
            new_string = alloc_str(s, new_string);
        }
    }

    return new_string;
}
