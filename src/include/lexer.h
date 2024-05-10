#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

enum {
    // keywords
    T_EXIT,

    // operators, symbols
    T_LPARAN,
    T_RPARAN,
    T_SEMI,

    // lit
    T_INTLIT,
    T_EOF
};

typedef struct TOKEN_STRUCT
{
    int token_type;
    char* value;
    int intvalue;
} token_T;

typedef struct LEXER_STRUCT
{
    char* source_file;
    ssize_t len;
    ssize_t index;
    char current_char;
    // TODO: line and column
} lexer_T;

token_T* init_token(int token_type, char* value);
lexer_T* init_lexer(char* source_file);
token_T* next_token(lexer_T* lexer);

#endif
