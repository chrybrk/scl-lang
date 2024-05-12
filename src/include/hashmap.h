#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

#define DEF_SIZE 1024

struct hash_pair
{
    char* key;
    ssize_t value;
};

typedef struct HASHMAP_STRUCT
{
    ssize_t size;
    struct hash_pair** buffer;
} hashmap_T;

hashmap_T* init_hashmap(ssize_t size);
void hashmap_insert(hashmap_T* hashmap, char* key, ssize_t value);
struct hash_pair* hashmap_find(hashmap_T* hashmap, char* key);

#endif
