#include "include/gen.h"
#include <string.h>

static char* reg[] = { "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
static char* regb[] = { "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" };
static char* regw[] = { "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" };
static char* regd[] = { "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d" };
static char* gen_reg_64[] = { "rax", "rbx", "rcx", "rdx" };
static char* gen_reg_32[] = { "eax", "ebx", "ecx", "edx" };

int free_gen_reg[4] = { 0 };
int free_reg[8] = { 0 };

char* from_gen_reg(char* var[])
{
    for (int i = 0; i < 4; i++)
    {
        if (free_gen_reg[i] == 0)
        {
            free_gen_reg[i] = 1;
            return var[i];
        }
    }

    return NULL;
}

char* from_reg(char* var[])
{
    for (int i = 0; i < 8; i++)
    {
        if (free_reg[i] == 0)
        {
            free_reg[i] = 1;
            return var[i];
        }
    }

    return NULL;
}

char* get_free_reg(char size)
{
    switch (size)
    {
        case 1: return from_reg(regb);
        case 2: return from_reg(regw);
        case 4: return from_reg(regd);
        case 8: return from_reg(reg);
    }

    return NULL;
}

// TODO: make it specific, so only used one will be freed not every other reg.
void free_regs()
{
    for (int i = 0; i < 8; i++) free_reg[i] = 0;
}

char get_data_type_size(int kind)
{
    switch (kind)
    {
        case CHAR: return 1;
        case I16: return 2;
        case I32: return 4;
        case I64: return 8;
    }

    return -1;
}

char* get_data_type_value(char size)
{
    switch (size)
    {
        case 1: return "byte";
        case 2: return "word";
        case 4: return "dword";
        case 8: return "qword";
    }

    return NULL;
}

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
    fprintf(gen->output, "format ELF64 executable\n");
    fprintf(gen->output, "segment readable executable\n");
    fprintf(gen->output, "entry _start\n");
    fprintf(gen->output, "_start:\n");
    fprintf(gen->output, "\tpush rbp\n");
    fprintf(gen->output, "\tmov rbp, rsp\n");

    if (root->ast_type == AST_STATEMENT)
        for (ssize_t i = 0; i < root->lst->index; i++) gen_statement(gen, (ast_T*)root->lst->buffer[i]);

    fprintf(gen->output, "\tpop rbp\n");
    fclose(gen->output);
}

void gen_exit(gen_T* gen, ast_T* node)
{
    fprintf(gen->output, "\tmov rax, 60\n");
    fprintf(gen->output, "\tmov rdi, ");
    gen_statement(gen, node->node);
    fprintf(gen->output, "\tsyscall\n");
}

void gen_expr(gen_T* gen, ast_T* node)
{
    switch (node->token->token_type)
    {
        case T_INTLIT: fprintf(gen->output, "%d\n", node->token->intvalue); break;
        case T_IDENT:
        {
            struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
            if (value)
            {
                struct stack_variable* v = gen->vars->buffer[value->value];
                fprintf(gen->output, "[rbp - %ld]\n", v->index);
            }
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
        char data_type_size = get_data_type_size(node->data_type); // TODO: check for -1
        char* data_type_value = get_data_type_value(data_type_size); // TODO: check for NULL
        gen->last_stack_index += data_type_size;

        if (((ast_T*)node->node)->token->token_type == T_IDENT)
        {
            char* r = get_free_reg(data_type_size);
            fprintf(gen->output, "\tmov %s, ", r);
            gen_statement(gen, node->node);
            fprintf(gen->output, "\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, r);
            free_regs();
        }
        else
        {
            fprintf(gen->output, "\tmov %s [rbp - %ld], ", data_type_value, gen->last_stack_index);
            gen_statement(gen, node->node);
        }

        struct stack_variable* var = calloc(1, sizeof(struct stack_variable));
        var->data_type = node->data_type;
        var->identifier = node->token->value;
        var->index = gen->last_stack_index;

        hashmap_insert(gen->hashmap, node->token->value, gen->size_of_stack);
        array_push(gen->vars, var);
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
