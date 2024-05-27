#ifndef GEN_H
#define GEN_H

#include "parser.h"
#include "array.h"
#include "hashmap.h"

struct stack_variable
{
    char* identifier;
    char data_type;
    ssize_t index;
    token_T* token;
};

struct function_writeable
{
    char* name;
    char* preamble;
    char* content;
    char* postamble;
    ssize_t size_of_stack;
    ssize_t last_stack_index;
};

typedef struct GEN_STRUCT
{
    FILE* output;
    hashmap_T* hashmap;
    hashmap_T* extern_hashmap;
    array_T* vars;
    array_T* functions;
    array_T* strings;
} gen_T;

gen_T* init_gen(char* output_pathname);
void gen_program(gen_T* gen, ast_T* root);
void gen_statement(gen_T* gen, ast_T* next_node);

#endif
