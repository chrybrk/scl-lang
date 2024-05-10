#include <stdio.h>
#include <stdlib.h>
#include "include/io.h"

char* read_file(const char* pathname)
{
    FILE* file = fopen(pathname, "rb");
    ssize_t len;
    fseek(file, 0L, SEEK_END);
    len = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char *buffer = calloc(1, sizeof(char) * len);
    fread(buffer, sizeof(char), len, file);
    fclose(file);
    return buffer;
}
