#include "include/gen.h"
#include <string.h>

gen_T* init_gen(char* output_pathname)
{
    gen_T* gen = calloc(1, sizeof(struct GEN_STRUCT));
    gen->output = fopen(output_pathname, "w");
    gen->size_of_stack = 0;
    gen->hashmap = init_hashmap(DEF_SIZE);
    gen->vars = init_array(sizeof(struct stack_variable));
    return gen;
}

void gen_program(gen_T* gen, ast_T* root)
{
    fprintf(gen->output, "%s", "\tglobal _start\n");
    fprintf(gen->output, "%s", "\tsection .text\n");
    fprintf(gen->output, "%s", "_start:\n");

    if (root->ast_type == AST_STATEMENT)
        for (ssize_t i = 0; i < root->lst->index; i++) gen_statement(gen, (ast_T*)root->lst->buffer[i]);

    fclose(gen->output);
}

void gen_exit(gen_T* gen, ast_T* node)
{
    fprintf(gen->output, "%s", "\tmov rax, 60\n");
    fprintf(gen->output, "%s", "\tmov rdi, ");
    gen_statement(gen, node->node);
    fprintf(gen->output, "%s", "\tsyscall\n");
}

void gen_expr(gen_T* gen, ast_T* node)
{
    switch (node->token->token_type)
    {
        case T_INTLIT: fprintf(gen->output, "%d\n", node->token->intvalue); break;
        case T_IDENT:
        {
            struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
            if (value) fprintf(gen->output, "qword [rsp + (%ld - %ld) * 8]\n", gen->size_of_stack - 1, value->value);
            else printf("[ERROR]: variable `%s` is not defined.\n", node->token->value);
            break;
        }
    };
}

void gen_let(gen_T* gen, ast_T* node)
{
    /*
     * find var_ident in hashmap
     * if not then create and insert(var_ident, index)
     * else err_already_exist
    */

    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
    if (!value)
    {
        struct stack_variable var = {
            .identifier = node->token->value,
            .data_type = node->data_type
        };

        fprintf(gen->output, "\tpush ");
        gen_statement(gen, node->node);

        hashmap_insert(gen->hashmap, node->token->value, gen->size_of_stack);
        array_push(gen->vars, &var);
        gen->size_of_stack++;
    }
    else printf("[ERROR]: variable already defined, '%s'.\n", node->token->value), exit(1);
}

void gen_statement(gen_T* gen, ast_T* next_node)
{
    switch (next_node->ast_type)
    {
        case AST_EXIT: gen_exit(gen, next_node); break;
        case AST_EXPR: gen_expr(gen, next_node); break;
        case AST_LET: gen_let(gen, next_node); break;
    }
}
