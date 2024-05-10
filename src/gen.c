#include "include/gen.h"

void gen_exit(ast_T* root)
{
    printf("    mov rax, 60\n");
    printf("    mov rdi, ");
    gen_gen(root->expr_node);
    printf("    syscall\n");
}

void gen_expr(ast_T* root)
{
    printf("%d\n", root->token->intvalue);
}

void gen_gen(ast_T* root)
{
    switch (root->ast_type)
    {
        case AST_EXIT: gen_exit(root); break;
        case AST_EXPR: gen_expr(root); break;
    }
}
