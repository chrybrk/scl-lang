#include <string.h>
#include <stdarg.h>
#include "include/utils.h"
#include "include/gen.h"
#include "include/error.h"
#include "include/global.h"

static char* r_arg[] = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9",
    "edi", "esi", "edx", "ecx", "r8d", "r9d",
    "di", "si", "dx", "cx", "r8w", "r9w"
};

static char* r_gen[] = {
    "rax", "rbx", "rcx", "rdx",
    "eax", "ebx", "ecx", "edx",
    "ax", "bx", "cx", "dx"
};

static char* r_reg[] = {
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d",
    "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w",
    "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};

static int arg_counter = 0;
static int gen_counter = 0;
static int reg_counter = 0;

enum {
    RT_arg = 0,
    RT_gen,
    RT_reg
};

void free_register()
{
    arg_counter = 0;
    gen_counter = 0;
    reg_counter = 0;
}

char* get_register(char size, int reg_type)
{
    switch (reg_type)
    {
        case RT_arg:
        {
            arg_counter++;

            if (arg_counter - 1 < 6)
            {
                switch (size)
                {
                    case 8: return r_arg[6 * 0 + (arg_counter - 1)];
                    case 4: return r_arg[6 * 1 + (arg_counter - 1)];
                    case 2:
                    case 1: return r_arg[6 * 2 + (arg_counter - 1)];
                };
            }

            break;
        }

        case RT_gen:
        {
            gen_counter++;

            if (gen_counter - 1 < 4)
            {
                switch (size)
                {
                    case 8: return r_gen[4 * 0 + (gen_counter - 1)];
                    case 4: return r_gen[4 * 1 + (gen_counter - 1)];
                    case 2:
                    case 1: return r_gen[4 * 2 + (gen_counter - 1)];
                };
            }

            break;
        }

        case RT_reg:
        {
            reg_counter++;

            if (reg_counter - 1 < 8)
            {
                switch (size)
                {
                    case 8: return r_reg[8 * 0 + (reg_counter - 1)];
                    case 4: return r_reg[8 * 1 + (reg_counter - 1)];
                    case 2: return r_reg[8 * 2 + (reg_counter - 1)];
                    case 1: return r_reg[8 * 3 + (reg_counter - 1)];
                };
            }

            break;
        }
    }

    return NULL;
}

char get_data_type_size(int kind)
{
    switch (kind)
    {
        case CHAR: return 1;
        case I16: return 2;
        case I32: return 4;
        case I64:
        case STRING: return 8;
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
    gen->functions = init_array(sizeof(struct function_writeable));
    gen->strings = init_array(sizeof(char*));
    return gen;
}

void gen_program(gen_T* gen, ast_T* root)
{
    fprintf(gen->output, "format ELF64\n");
    fprintf(gen->output, "section '.text' executable\n");
    fprintf(gen->output, "public _start\n");

    struct function_writeable* _start = calloc(1, sizeof(struct function_writeable));
    _start->preamble = alloc_str("\tpush rbp\n\tmov rbp, rsp\n", _start->preamble);
    _start->name = "_start";

    array_push(gen->functions, _start);

    if (root->ast_type == AST_STATEMENT)
        for (ssize_t i = 0; i < root->lst->index; i++) gen_statement(gen, (ast_T*)root->lst->buffer[i]);

    for (ssize_t index = 0; index < gen->functions->index; index++)
    {
        // FIXME: size_of_stack and last_stack_index, has to be properties of function not global.
        struct function_writeable* fnc = gen->functions->buffer[index];
        fnc->preamble = alloc_str(writef("\tsub rsp, %ld\n", gen->last_stack_index), fnc->preamble);
        fnc->postamble = alloc_str(writef("\tadd rsp, %ld\n\tpop rbp\n", gen->last_stack_index), fnc->postamble);
        fprintf(gen->output, "%s:\n", fnc->name);
        fprintf(gen->output, "%s", fnc->preamble);
        fprintf(gen->output, "%s", fnc->content);
        fprintf(gen->output, "%s", fnc->postamble);
    }

    fprintf(gen->output, "\nsection '.data' writeable\n");

    for (ssize_t index = 0; index < gen->strings->index; index++)
        fprintf(gen->output, "string_%ld: db \"%s\", 0\n", index, (char*)gen->strings->buffer[index]);

    fclose(gen->output);
}

void gen_exit(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    gen_statement(gen, node->node);
    fnc->content = alloc_str("\tmov rdi, rax\n", fnc->content);
    fnc->content = alloc_str("\tmov rax, 60\n", fnc->content);
    fnc->content = alloc_str("\tsyscall\n", fnc->content);

}

void gen_expr(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    switch (node->token->token_type)
    {
        case T_INTLIT: fnc->content = alloc_str(writef("\tmov rax, %d\n", node->token->intvalue), fnc->content); break;
        case T_IDENT:
        {
            struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
            if (value)
            {
                struct stack_variable* v = gen->vars->buffer[value->value];
                fnc->content = alloc_str(writef("mov rax, [rbp - %ld]\n", v->index), fnc->content);
            }
            else init_error_with_token(node->token, E_FAILED, writef("variable `%s` is not defined.\n", node->token->value));
            break;
        }
        case T_STRING:
        {
            fnc->content = alloc_str(writef("\tmov rax, string_%d\n", gen->strings->index), fnc->content);
            array_push(gen->strings, node->token->value);
            break;
        }
    };
}

void gen_let(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
    ast_T* next_node = node->node;

    if (!value)
    {
        char data_type_size = get_data_type_size(node->data_type); // TODO: check for -1
        char* data_type_value = get_data_type_value(data_type_size); // TODO: check for NULL

        gen->last_stack_index += data_type_size;

        if (next_node->token->token_type == T_IDENT && next_node->ast_type == AST_EXPR)
        {
            struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, next_node->token->value)->value];
            if (!r_var) init_error_with_token(next_node->token, E_FAILED, writef("variable not defined, '%s'.\n", next_node->token->value));
            char data_type_size_for_rop = get_data_type_size(r_var->data_type);
            if ((int)data_type_size < (int)data_type_size_for_rop)
                init_error_with_token(node->token, E_WARN, writef("`%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n", r_var->identifier, node->token->value));

            char* r = get_register(data_type_size, RT_reg);
            char* gr = get_register(data_type_size, RT_gen);
            gen_statement(gen, node->node);
            fnc->content = alloc_str(writef("\tmov %s, %s\n", r, gr), fnc->content);
            fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, r), fnc->content);
            free_register();
        }
        else
        {
            gen_statement(gen, node->node);
            char* gr = get_register(data_type_size, RT_gen);
            fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, gr), fnc->content);
            free_register();
        }

        struct stack_variable* var = calloc(1, sizeof(struct stack_variable));
        var->data_type = node->data_type;
        var->identifier = node->token->value;
        var->index = gen->last_stack_index;
        var->token = node->token;

        // FIXME: redundant use case of `gen->size_of_stack`, when instead we can just use, `array->index`.

        hashmap_insert(gen->hashmap, node->token->value, gen->size_of_stack);
        array_push(gen->vars, var);
        gen->size_of_stack++;
    }
    else
    {
        struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, node->token->value)->value];
        init_error_with_token(node->token, E_FAILED, writef("variable `%s` already defined.", node->token->value));
        init_error_with_token(r_var->token, E_FAILED, writef("variable `%s` defined here first.", node->token->value));
    }

}

void gen_extern(gen_T* gen, ast_T* node)
{
    fprintf(gen->output, "extrn %s\n", node->token->value);
}

void gen_var(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);

    if (value)
    {
        struct stack_variable* var = gen->vars->buffer[value->value];
        char data_type_size_for_lop = get_data_type_size(var->data_type);
        ast_T* next_node = node->node;

        if (next_node->token->token_type == T_IDENT)
        {
            struct hash_pair* hp = hashmap_find(gen->hashmap, next_node->token->value);
            if (hp)
            {
                struct stack_variable* r_var = gen->vars->buffer[hp->value];
                if (!r_var) init_error_with_token(next_node->token, E_FAILED, writef("variable not defined, '%s'.\n", next_node->token->value));

                char data_type_size_for_rop = get_data_type_size(r_var->data_type);
                if ((int)data_type_size_for_lop < (int)data_type_size_for_rop)
                    printf("[WARN]: `%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n", r_var->identifier, node->token->value);

                char* r = get_register(data_type_size_for_lop, RT_reg);
                gen_statement(gen, node->node);
                fnc->content = alloc_str(writef("\tmov %s, rax\n", r), fnc->content);
                fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n", get_data_type_value(data_type_size_for_lop), var->index, r), fnc->content);
                free_register();
            }
            else init_error_with_token(next_node->token, E_FAILED, writef("variable not defined, '%s'.\n", next_node->token->value));
        }
        else
        {
            gen_statement(gen, node->node);
            fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], rax\n", get_data_type_value(data_type_size_for_lop), var->index), fnc->content);
        }
    }
    else init_error_with_token(node->token, E_FAILED, writef("variable not defined, `%s`.\n", node->token->value));
}

void gen_call(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    gen_statement(gen, node->node);
    fnc->content = alloc_str("\tmov rdi, rax\n", fnc->content);
    fnc->content = alloc_str(writef("\tcall %s\n", node->token->value), fnc->content);
}

void gen_statement(gen_T* gen, ast_T* next_node)
{
    switch (next_node->ast_type)
    {
        case AST_EXIT: gen_exit(gen, next_node); break;
        case AST_EXPR: gen_expr(gen, next_node); break;
        case AST_LET: gen_let(gen, next_node); break;
        case AST_EXTERN: gen_extern(gen, next_node); break;
        case AST_VAR: gen_var(gen, next_node); break;
        case AST_CALL: gen_call(gen, next_node); break;
    }
}
