#include "include/hashmap.h"
#include <stdio.h>
#include <string.h>

hashmap_T* init_hashmap(ssize_t size)
{
    hashmap_T* hashmap = calloc(1, sizeof(struct HASHMAP_STRUCT));
    hashmap->size = size;
    hashmap->buffer = calloc(1, sizeof(struct hash_pair) * size);
    return hashmap;
}

void hashmap_insert(hashmap_T* hashmap, char* key, ssize_t value)
{
    // formula for selecting index.
    ssize_t unq_value = 0;
    for (size_t i = 0; i < strlen(key); i++) unq_value += (int)key[i];
    unq_value %= hashmap->size;

    struct hash_pair* from_buffer = hashmap->buffer[unq_value];
    struct hash_pair* new_value = calloc(1, sizeof(struct hash_pair));
    new_value->key = key;
    new_value->value = value;

    if (!from_buffer) hashmap->buffer[unq_value] = new_value;
    else
    {
        if (!strcmp(key, from_buffer->key))
        {
            printf("[ERROR]: hashmap: failed to insert `%s` into hashmap, it already exist.\n", key);
            exit(1);
        }

        for (ssize_t i = unq_value + 1; i < hashmap->size; i++)
        {
            from_buffer = hashmap->buffer[i];
            if (from_buffer)
            {
                if (!strcmp(key, from_buffer->key))
                {
                    printf("[ERROR]: hashmap: failed to insert `%s` into hashmap, it already exist.\n", key);
                    exit(1);
                }
            }
            else
            {
                hashmap->buffer[i] = new_value;
                break;
            }
        }

        printf("[ERROR]: hashmap: failed to insert `%s`, overflow.\n", key);
        exit(1);
    }
}

struct hash_pair* hashmap_find(hashmap_T* hashmap, char* key)
{
    // formula for selecting index.
    ssize_t unq_value = 0;
    for (size_t i = 0; i < strlen(key); i++) unq_value += (int)key[i];
    unq_value %= hashmap->size;

    struct hash_pair* from_buffer = hashmap->buffer[unq_value];

    if (from_buffer)
    {
        if (!strcmp(key, from_buffer->key)) return from_buffer;

        for (ssize_t i = unq_value + 1; i < hashmap->size; i++)
        {
            from_buffer = hashmap->buffer[i];
            if (from_buffer)
            {
                if (!strcmp(key, from_buffer->key)) return from_buffer;
            }
        }
    }

    return NULL;
}
