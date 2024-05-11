#include "include/gen.h"
#include <string.h>

void gen_exit(ast_T* root)
{
    fprintf(output, "%s", "\tmov rax, 60\n");
    fprintf(output, "%s", "\tmov rdi, ");
    gen_stmnt(root->node);
    fprintf(output, "%s", "\tsyscall\n");
}

void gen_expr(ast_T* root)
{
    fprintf(output, "%d\n", root->token->intvalue);
}

void gen_let(ast_T* root)
{
    // TODO: implement hashmap, it is SLOW!!
    for (ssize_t i = 0; i < gen_stack->index; i++)
    {
        if (!strcmp(root->token->value, ((struct stack_variable*)gen_stack->buffer[i])->identifier))
            printf("[ERROR]: variable already defined, `%s`.\n", root->token->value);
    }

    struct stack_variable var = {
        .identifier = root->token->value,
        .data_type = root->data_type
    };

    array_push(gen_stack, &var);
    fprintf(output, "\tpush ");
    gen_stmnt(root->node);
}

void gen_preamble(char* output_pathname)
{
    output = fopen(output_pathname, "w");
    gen_stack = init_array(sizeof(struct stack_variable));
    fprintf(output, "%s", "\tglobal _start\n");
    fprintf(output, "%s", "\tsection .text\n");
    fprintf(output, "%s", "_start:\n");
}

void gen_postamble()
{
    fclose(output); // was it that important??
}

void gen_stmnt(ast_T* stmnt)
{
    switch (stmnt->ast_type)
    {
        case AST_EXIT: gen_exit(stmnt); break;
        case AST_EXPR: gen_expr(stmnt); break;
        case AST_LET: gen_let(stmnt); break;
    }
}

void gen_program(ast_T* root)
{
    if (root->ast_type == AST_STATEMENT)
        for (ssize_t i = 0; i < root->lst->index; i++) gen_stmnt((ast_T*)root->lst->buffer[i]);
}
