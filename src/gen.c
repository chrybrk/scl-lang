#include "include/gen.h"
#include <string.h>
#include <stdarg.h>

// TODO: use local string, than directly writing everythin into output

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

char* write(char* fmt, ...)
{
    char* string;
    va_list args;
    va_start(args, fmt);
    vasprintf(&string, fmt, args);
    va_end(args);
    return string;
}

char* calculate_write_data(char* src, char* dst)
{
    if (dst == NULL)
    {
        dst = calloc(1, sizeof(char) * strlen(src) + 1);
        dst = strcpy(dst, src);
        return dst;
    }

    dst = realloc(dst, sizeof(char) * (strlen(dst) + strlen(src) + 2));
    dst = strcat(dst, src);
    return dst;
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
    _start->preamble = "\tpush rbp\n\tmov rbp, rsp\n";
    _start->postamble = "\tpop rbp";
    _start->name = "_start";

    array_push(gen->functions, _start);

    if (root->ast_type == AST_STATEMENT)
        for (ssize_t i = 0; i < root->lst->index; i++) gen_statement(gen, (ast_T*)root->lst->buffer[i]);

    for (ssize_t index = 0; index < gen->functions->index; index++)
    {
        struct function_writeable* fnc = gen->functions->buffer[index];
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
    fnc->content = calculate_write_data("\tmov rax, 60\n", fnc->content);
    fnc->content = calculate_write_data("\tmov rdi, ", fnc->content);
    gen_statement(gen, node->node);
    fnc->content = calculate_write_data("\tsyscall\n", fnc->content);

}

void gen_expr(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    switch (node->token->token_type)
    {
        case T_INTLIT: fnc->content = calculate_write_data(write("%d\n", node->token->intvalue), fnc->content); break;
        case T_IDENT:
        {
            struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
            if (value)
            {
                struct stack_variable* v = gen->vars->buffer[value->value];
                fnc->content = calculate_write_data(write("[rbp - %ld]\n", v->index), fnc->content);
            }
            else printf("[ERROR]: variable `%s` is not defined.\n", node->token->value);
            break;
        }
        case T_STRING:
        {
            fnc->content = calculate_write_data(write("string_%d\n", gen->strings->index), fnc->content);
            array_push(gen->strings, node->token->value);
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
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];

    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);
    if (!value)
    {
        char data_type_size = get_data_type_size(node->data_type); // TODO: check for -1
        char* data_type_value = get_data_type_value(data_type_size); // TODO: check for NULL
        gen->last_stack_index += data_type_size;

        if (((ast_T*)node->node)->token->token_type == T_IDENT)
        {
            struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, ((ast_T*)node->node)->token->value)->value];
            if (!r_var) printf("[ERROR]: variable not defined, '%s'.\n", ((ast_T*)node->node)->token->value), exit(1);
            char data_type_size_for_rop = get_data_type_size(r_var->data_type);
            if ((int)data_type_size < (int)data_type_size_for_rop)
                printf("[WARN]: `%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n", r_var->identifier, node->token->value);

            char* r = get_free_reg(data_type_size);
            fnc->content = calculate_write_data(write("\tmov %s, ", r), fnc->content);
            gen_statement(gen, node->node);
            fnc->content = calculate_write_data(write("\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, r), fnc->content);
            free_regs();
        }
        else
        {
            fnc->content = calculate_write_data(write("\tmov %s [rbp - %ld], ", data_type_value, gen->last_stack_index), fnc->content);
            gen_statement(gen, node->node);
        }

        struct stack_variable* var = calloc(1, sizeof(struct stack_variable));
        var->data_type = node->data_type;
        var->identifier = node->token->value;
        var->index = gen->last_stack_index;

        // FIXME: redundant use case of `gen->size_of_stack`, when instead we can just use, `array->index`.

        hashmap_insert(gen->hashmap, node->token->value, gen->size_of_stack);
        array_push(gen->vars, var);
        gen->size_of_stack++;
    }
    else printf("[ERROR]: variable already defined, '%s'.\n", node->token->value), exit(1);
}

void gen_extern(gen_T* gen, ast_T* node)
{
    fprintf(gen->output, "extrn %s\n", node->token->value);
}

void gen_var(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);

    // TODO: Please, just fucking fix it. It is so fucking messy.

    if (value)
    {
        struct stack_variable* var = gen->vars->buffer[value->value];
        char data_type_size_for_lop = get_data_type_size(var->data_type);

        if (((ast_T*)node->node)->token->token_type == T_IDENT)
        {
            // FIXME: hashmap_find... thing has to be seperated to be tested if it is (nil).
            struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, ((ast_T*)node->node)->token->value)->value];
            if (!r_var) printf("[ERROR]: variable not defined, '%s'.\n", ((ast_T*)node->node)->token->value), exit(1);
            char data_type_size_for_rop = get_data_type_size(r_var->data_type);
            if ((int)data_type_size_for_lop < (int)data_type_size_for_rop)
                printf("[WARN]: `%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n", r_var->identifier, node->token->value);

            char* r = get_free_reg(data_type_size_for_lop);
            fnc->content = calculate_write_data(write("\tmov %s, ", r), fnc->content);
            gen_statement(gen, node->node);
            fnc->content = calculate_write_data(write("\tmov %s [rbp - %ld], %s\n", get_data_type_value(data_type_size_for_lop), var->index, r), fnc->content);
            free_regs();
        }
        else
        {
            fnc->content = calculate_write_data(write("\tmov %s [rbp - %ld], ", get_data_type_value(data_type_size_for_lop), var->index), fnc->content);
            gen_statement(gen, node->node);
        }
    }
    else printf("[ERROR]: variable not defined, `%s`.\n", node->token->value), exit(1);
}

void gen_call(gen_T* gen, ast_T* node)
{
    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    fnc->content = calculate_write_data("\tmov rdi, ", fnc->content);
    gen_statement(gen, node->node);
    fnc->content = calculate_write_data(write("\tcall %s\n", node->token->value), fnc->content);
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
