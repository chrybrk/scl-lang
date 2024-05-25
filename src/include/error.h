#ifndef ERROR_H
#define ERROR_H

#include "lexer.h"
#include "array.h"
#include "utils.h"

enum {
    E_LOG,
    E_WARN,
    E_FAILED
};

typedef struct ERROR_STRUCT
{
    int e_type;
    char* error;
} error_T;

void init_error(int e_type, char* error);
void init_error_with_lexer(lexer_T* lexer, int e_type, char* error);
void init_error_with_token(token_T* token, int e_type, char* error);
void error_flush();

#endif
