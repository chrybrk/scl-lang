#include "include/error.h"
#include "include/global.h"
#include <stdio.h>
#include <stdlib.h>

void init_error(int e_type, char* error)
{
    error_T* er = calloc(1, sizeof(struct ERROR_STRUCT));
    er->e_type = e_type;
    er->error = error;

    if (scl_error_list) array_push(scl_error_list, er);
    else
    {
        scl_error_list = init_array(sizeof(struct ERROR_STRUCT));
        array_push(scl_error_list, er);
    }
}

void init_error_with_lexer(lexer_T* lexer, int e_type, char* error)
{
    char* new_error_message = NULL;
    new_error_message = alloc_str(writef("%s:%d:%d: %s\n%2d | %s\n",
                lexer->pathname,
                lexer->ln,
                lexer->clm,
                error,
                lexer->ln,
                scl_python_readlines->buffer[lexer->ln - 1]), new_error_message);

    init_error(e_type, new_error_message);
}

void init_error_with_token(token_T* token, int e_type, char* error)
{
    char* new_error_message = NULL;
    new_error_message = alloc_str(writef("%s:%d:%d: %s\n%2d | %s\n",
                scl_current_file,
                token->ln,
                token->clm,
                error,
                token->ln,
                scl_python_readlines->buffer[token->ln - 1]), new_error_message);

    init_error(e_type, new_error_message);
}

void error_flush()
{
    if (scl_error_list)
    {
        int if_failed = 0;
        for (ssize_t i = 0; i < scl_error_list->index; i++)
        {
            error_T* er = scl_error_list->buffer[i];
            switch (er->e_type)
            {
                case E_LOG: printf("[INFO]: %s\n", er->error); break;
                case E_WARN: printf("[WARN]: %s\n", er->error); break;
                case E_FAILED: printf("[ERROR]: %s\n", er->error); if_failed = 1; break;
            }
        }

        if (if_failed) exit(1);
    }
}
