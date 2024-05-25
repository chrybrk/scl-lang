#include "include/array.h"
#include <stdio.h>

array_T* init_array(ssize_t item_size)
{
    array_T* array = calloc(1, sizeof(struct ARRAY_STRUCT));
    array->buffer = calloc(1, item_size);
    array->index = 0;
    array->item_size = item_size;
    return array;
}

void array_push(array_T* array, void* item)
{
    array->index++;
    if (array->buffer) array->buffer = realloc(array->buffer, array->item_size * array->index);
    else array->buffer = calloc(1, array->item_size * array->index);
    array->buffer[array->index - 1] = item;
}
