#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "array.h"

enum {
    AST_EXIT,
    AST_EXPR,
    AST_LET,
    AST_STATEMENT
};

enum {
    CHAR,
    I16,
    I32,
    I64
};

typedef struct AST_STRUCT
{
    int ast_type;
    int data_type;
    void* node;
    array_T* lst;
    token_T* token;
} ast_T;

typedef struct PARSER_STRUCT
{
    lexer_T* lexer;
    token_T* current_token;
} parser_T;

ast_T* init_ast_node(ast_T* expr_node, int type);
ast_T* init_ast_list(int type);
ast_T* init_ast_stmnt(ast_T* expr_node, token_T* ident, int type);
ast_T* init_ast_expr(token_T* token);
parser_T* init_parser(lexer_T* lexer);
ast_T* parser_parse(parser_T* parser);

#endif
