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

char* get_register_noINC(char size, int reg_type)
{
    switch (reg_type)
    {
        case RT_arg:
        {
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
        case VOID: return 0;
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
    gen->hashmap = init_hashmap(DEF_SIZE);
    gen->extern_hashmap = init_hashmap(DEF_SIZE);
    gen->vars = init_array(sizeof(struct stack_variable));
    gen->functions = init_array(sizeof(struct function_writeable));
    gen->strings = init_array(sizeof(char*));
    return gen;
}

void gen_preamble(gen_T* gen)
{
    fprintf(gen->output, "format ELF64\n");
    fprintf(gen->output, "section '.text' executable\n");
    fprintf(gen->output, "public _start\n");
}

void gen_postamble(gen_T* gen)
{
    for (ssize_t index = 0; index < gen->functions->index; index++)
    {
        // FIXME: size_of_stack and last_stack_index, has to be properties of function not global.
        struct function_writeable* fnc = gen->functions->buffer[index];
        fnc->preamble = alloc_str(writef("\tsub rsp, %ld\n", fnc->last_stack_index), fnc->preamble);
        fnc->postamble = alloc_str(writef("\tadd rsp, %ld\n\tpop rbp\n", fnc->last_stack_index), fnc->postamble);
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

void gen_program(gen_T* gen, ast_T* root)
{
    gen_preamble(gen);

    struct function_writeable* _start = calloc(1, sizeof(struct function_writeable));
    _start->preamble = alloc_str("\tpush rbp\n\tmov rbp, rsp\n", _start->preamble);
    _start->name = "_start";

    array_push(gen->functions, _start);

    gen_statement(gen, root);
    gen_postamble(gen);
}

struct stack_variable* gen_find_variable_in_scope(const char* variable_name)
{
    return NULL;
}

void gen_exit(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    gen_statement(gen, node->node);
    fnc->content = alloc_str("\tpop rdi\n", fnc->content);
    fnc->content = alloc_str("\tmov rax, 60\n", fnc->content);
    fnc->content = alloc_str("\tsyscall\n", fnc->content);
}

void gen_expr(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    switch (node->token->token_type)
    {
        case T_INTLIT: fnc->content = alloc_str(writef("\tpush %d\n", node->token->intvalue), fnc->content); break;
        case T_IDENT:
        {
            struct hash_pair* value = hashmap_find(gen->hashmap, writef("%s%d", node->token->value, gen->current_label));
            if (value)
            {
                struct stack_variable* v = gen->vars->buffer[value->value];
                fnc->content = alloc_str(writef("\tmov rax, [rbp - %ld]\n\tpush rax\n", v->index), fnc->content);
            }
            else init_error_with_token(node->token, E_FAILED, writef("variable `%s` is not defined.\n", node->token->value));
            break;
        }
        case T_STRING:
        {
            fnc->content = alloc_str(writef("\tlea rax, [string_%d]\n\tpush rax\n", gen->strings->index), fnc->content);
            array_push(gen->strings, node->token->value);
            break;
        }
    };
}

void gen_variable_identifier(gen_T* gen, ast_T* node)
{
    // fnc holds current function we are in.
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];

    ast_T* next_node = node->node;

    // left variable
    char l_variable_size = get_data_type_size(node->data_type);
    char* l_variable_dt_value = get_data_type_value(l_variable_size);

    switch (next_node->ast_type)
    {
        case AST_EXPR:
        {
            // right variable information
            struct hash_pair* r_variable_index = hashmap_find(gen->hashmap, writef("%s%d", next_node->token->value, gen->current_label));
            struct stack_variable* r_variable = NULL;

            if (r_variable_index) r_variable = gen->vars->buffer[r_variable_index->value];

            if (!r_variable_index || !r_variable)
                init_error_with_token(
                        next_node->token,
                        E_FAILED,
                        writef("variable not defined, '%s'.\n",
                            next_node->token->value)
                ), error_flush();


            char r_variable_size = get_data_type_size(r_variable->data_type);

            // if they are not the same size, or left variable size is lower then we have an warning to generate.
            if ((int)l_variable_size < (int)r_variable_size)
                init_error_with_token(
                        node->token,
                        E_WARN,
                        writef("`%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n",
                            r_variable->identifier,
                            node->token->value)
                );

            break;
        }
        case AST_CALL:
        {
            struct hash_pair* fnc_call = hashmap_find(gen->extern_hashmap, writef("%s%d", node->token->value, gen->current_label));
            if (fnc_call)
            {
                if (fnc_call->value == VOID) init_error_with_token(node->token, E_WARN, writef("function `%s` has return type of void.", next_node->token->value));

                char r_function_size = get_data_type_size(fnc_call->value);
                if ((int)l_variable_size < (int)r_function_size)
                    init_error_with_token(
                            node->token,
                            E_WARN,
                            writef("function `%s` return size does not match, variable `%s` has smaller size.",
                                next_node->token->value,
                                node->token->value)
                    );
            }

            break;
        }
    }

    char* r = get_register(l_variable_size, RT_reg);
    char* gr = get_register(l_variable_size, RT_gen);
    char* grnoINC = get_register_noINC(8, RT_gen);

    gen_statement(gen, next_node);

    fnc->content = alloc_str(writef("\tpop %s\n\tmov %s, %s\n", grnoINC, r, gr), fnc->content);
    fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n",
                l_variable_dt_value,
                fnc->last_stack_index,
                r),
            fnc->content);

    free_register();
}

void gen_variable_selection(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    ast_T* next_node = node->node;

    char l_variable_size = get_data_type_size(node->data_type);
    char* l_variable_dt_value = get_data_type_value(l_variable_size);

    fnc->last_stack_index += l_variable_size;

    if (next_node->token)
        switch (next_node->token->token_type)
        {
            case T_IDENT: gen_variable_identifier(gen, node); break;
            case T_INTLIT:
            {
                gen_statement(gen, node->node);

                char* gr = get_register(l_variable_size, RT_gen);
                char* grnoINC = get_register_noINC(8, RT_gen);

                fnc->content = alloc_str(
                        writef("\tpop %s\n\tmov %s [rbp - %ld], %s\n",
                            grnoINC,
                            l_variable_dt_value,
                            fnc->last_stack_index,
                            gr),
                        fnc->content);

                free_register();
                break;
            }
            case T_STRING:
            {
                gen_statement(gen, node->node);

                char* gr = get_register(l_variable_size, RT_gen);
                char* grnoINC = get_register_noINC(8, RT_gen);

                fnc->content = alloc_str(
                        writef("\tpop %s\n\tmov %s [rbp - %ld], %s\n",
                            grnoINC,
                            l_variable_dt_value,
                            fnc->last_stack_index,
                            gr),
                        fnc->content);

                free_register();

                break;
            }
        }
    else
    {
        gen_statement(gen, node->node);

        char* gr = get_register(l_variable_size, RT_gen);
        char* grnoINC = get_register_noINC(8, RT_gen);

        fnc->content = alloc_str(
                writef("\tpop %s\n\tmov %s [rbp - %ld], %s\n",
                    grnoINC,
                    l_variable_dt_value,
                    fnc->last_stack_index,
                    gr),
                fnc->content);

        free_register();
    }
}

void gen_let(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    struct hash_pair* value = hashmap_find(gen->hashmap, writef("%s%d", node->token->value, gen->current_label));

    if (!value || (value && ((struct stack_variable*)gen->vars->buffer[value->value])->scope > gen->current_label))
    {
        gen_variable_selection(gen, node);

        struct stack_variable* var = calloc(1, sizeof(struct stack_variable));
        var->data_type = node->data_type;
        var->identifier = node->token->value;
        var->index = fnc->last_stack_index;
        var->token = node->token;
        var->scope = gen->current_label;

        hashmap_insert(gen->hashmap, writef("%s%d", var->identifier, var->scope), fnc->size_of_stack);
        array_push(gen->vars, var);
        fnc->size_of_stack++;
    }
    else
    {
        struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, writef("%s%d", node->token->value, gen->current_label))->value];
        init_error_with_token(node->token, E_FAILED, writef("variable `%s` already defined.", node->token->value));
        init_error_with_token(r_var->token, E_FAILED, writef("variable `%s` defined here first.", node->token->value));
    }
}

void gen_var(gen_T* gen, ast_T* node)
{
    struct hash_pair* value = hashmap_find(gen->hashmap, writef("%s%d", node->token->value, gen->current_label));

    if (value) gen_variable_selection(gen, node);
    else init_error_with_token(node->token, E_FAILED, writef("variable not defined, `%s`.\n", node->token->value));
}

void gen_extern(gen_T* gen, ast_T* node)
{
    struct hash_pair* extrn = hashmap_find(gen->extern_hashmap, node->token->value);
    if (extrn) init_error_with_token(node->token, E_FAILED, "multiple definition of extern.");
    else
    {
        fprintf(gen->output, "extrn %s\n", node->token->value);
        hashmap_insert(gen->extern_hashmap, node->token->value, node->data_type);
    }
}

void gen_call(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];

    for (ssize_t i = 0; i < node->lst->index; i++)
    {
        char* ar = get_register(8, RT_arg);
        gen_statement(gen, (ast_T*)node->lst->buffer[i]);
        fnc->content = alloc_str(writef("\tpop %s\n", ar), fnc->content);
    }

    free_register();
    fnc->content = alloc_str(writef("\tcall %s\n", node->token->value), fnc->content);
}

void gen_binop(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];

    ast_T* ln = node->left_node;
    ast_T* rn = node->right_node;

    gen_statement(gen, ln);
    gen_statement(gen, rn);

    switch (node->op)
    {
        case T_PLUS:
            fnc->content = alloc_str("\tpop rdx\n", fnc->content);
            fnc->content = alloc_str("\tpop rax\n", fnc->content);
            fnc->content = alloc_str("\tadd rax, rdx\n", fnc->content);
            fnc->content = alloc_str("\tpush rax\n", fnc->content);
            break;

        case T_MINUS:
            fnc->content = alloc_str("\tpop rdx\n", fnc->content);
            fnc->content = alloc_str("\tpop rax\n", fnc->content);
            fnc->content = alloc_str("\tsub rax, rdx\n", fnc->content);
            fnc->content = alloc_str("\tpush rax\n", fnc->content);
            break;

        case T_STAR:
            fnc->content = alloc_str("\tpop rdx\n", fnc->content);
            fnc->content = alloc_str("\tpop rax\n", fnc->content);
            fnc->content = alloc_str("\tmul rdx\n", fnc->content);
            fnc->content = alloc_str("\tpush rax\n", fnc->content);
            break;

        case T_FSLASH:
            fnc->content = alloc_str("\txor rdx, rdx\n", fnc->content);
            fnc->content = alloc_str("\tpop rcx\n", fnc->content);
            fnc->content = alloc_str("\tpop rax\n", fnc->content);
            fnc->content = alloc_str("\tdiv rcx\n", fnc->content);
            fnc->content = alloc_str("\tpush rax\n", fnc->content);
            break;

        case T_MODULO:
            fnc->content = alloc_str("\txor rdx, rdx\n", fnc->content);
            fnc->content = alloc_str("\tpop rcx\n", fnc->content);
            fnc->content = alloc_str("\tpop rax\n", fnc->content);
            fnc->content = alloc_str("\tdiv rcx\n", fnc->content);
            fnc->content = alloc_str("\tmov rax, rdx\n", fnc->content);
            fnc->content = alloc_str("\tpush rax\n", fnc->content);
            break;
    }

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
        case AST_BINOP: gen_binop(gen, next_node); break;
        case AST_STATEMENT:
        {
            int l_label = gen->current_label;
            gen->current_label = next_node->op;

            for (ssize_t i = 0; i < next_node->lst->index; i++)
            {
                ast_T* ast = next_node->lst->buffer[i];
                gen_statement(gen, ast);
            }

            gen->current_label = l_label;
            break;
        }
    }
}
