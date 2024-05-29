#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

enum {
    // keywords
    T_EXIT,
    T_LET,
    T_EXTERN,

    // operators, symbols
    T_LPARAN,
    T_RPARAN,
    T_SEMI,
    T_COLON,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,

    // lit
    T_INTLIT,
    T_IDENT,
    T_STRING,
    T_EOF
};

typedef struct TOKEN_STRUCT
{
    int token_type;
    int intvalue;
    int ln;
    int clm;
    char* value;
} token_T;

typedef struct LEXER_STRUCT
{
    char* pathname;
    char* source_file;
    ssize_t len;
    ssize_t index;
    char current_char;
    ssize_t ln;
    ssize_t clm;
    hashmap_T* hashmap;
} lexer_T;

// initiate token
token_T* init_token(int token_type, char* value, int ln, int clm);

// initiate lexer
lexer_T* init_lexer(char* source_file, char* pathname);

// it will give next token upon calling
token_T* next_token(lexer_T* lexer);

// print the token from its type.
char* print_token(int token_type);

#endif
