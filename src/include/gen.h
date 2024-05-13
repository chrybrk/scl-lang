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
};

typedef struct GEN_STRUCT
{
    FILE* output;
    ssize_t size_of_stack;
    ssize_t last_stack_index;
    hashmap_T* hashmap;
    array_T* vars;
} gen_T;

gen_T* init_gen(char* output_pathname);
void gen_program(gen_T* gen, ast_T* root);
void gen_statement(gen_T* gen, ast_T* next_node);

#endif
