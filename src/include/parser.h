#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum {
    AST_EXIT,
    AST_EXPR
};

typedef struct AST_STRUCT
{
    int ast_type;
    
    // if exit
    void* expr_node;

    // if expr
    token_T* token;
} ast_T;

typedef struct PARSER_STRUCT
{
    lexer_T* lexer;
    token_T* current_token;
} parser_T;

ast_T* init_ast_exit(ast_T* expr_node);
ast_T* init_ast_expr(token_T* token);
parser_T* init_parser(lexer_T* lexer);
ast_T* parser_parse(parser_T* parser);

#endif
