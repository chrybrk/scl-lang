#ifndef GEN_H
#define GEN_H

#include "parser.h"
#include "array.h"

struct stack_variable
{
    char* identifier;
    int data_type;
};

static FILE* output;
static array_T* gen_stack;

void gen_preamble(char* output_pathname);
void gen_stmnt(ast_T* stmnt);
void gen_program(ast_T* root);
void gen_postamble();

#endif
