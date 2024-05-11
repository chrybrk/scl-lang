#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct ARRAY_STRUCT
{
    void** buffer;
    ssize_t index;
    ssize_t item_size;
} array_T;

array_T* init_array(ssize_t item_size);
void array_push(array_T* array, void* item);

#endif
