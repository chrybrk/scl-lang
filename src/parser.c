#include <string.h>
#include "include/parser.h"

ast_T* init_ast_node(ast_T* node, int type)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->node = node;
    ast->lst = NULL;
    ast->token = NULL;
    return ast;
}

ast_T* init_ast_list(int type)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->node = NULL;
    ast->lst = init_array(sizeof(struct AST_STRUCT));
    ast->token = NULL;
    return ast;
}

ast_T* init_ast_stmnt(ast_T* expr_node, token_T* ident, int type)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->node = expr_node;
    ast->lst = NULL;
    ast->token = ident;
    return ast;
}

ast_T* init_ast_expr(token_T* token)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = AST_EXPR;
    ast->node = NULL;
    ast->lst = NULL;
    ast->token = token;
    return ast;
}

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = next_token(parser->lexer);
    parser->hashmap = init_hashmap(DEF_SIZE);

    hashmap_insert(parser->hashmap, "char", CHAR);
    hashmap_insert(parser->hashmap, "i16", I16);
    hashmap_insert(parser->hashmap, "i32", I32);
    hashmap_insert(parser->hashmap, "i64", I64);

    return parser;
}

token_T* parser_token_consume(parser_T* parser, int expected_type)
{
    if (parser->current_token->token_type == expected_type)
    {
        token_T* token = parser->current_token;
        parser->current_token = next_token(parser->lexer);
        return token;
    }

    printf("[ERROR]: Parser failed to consume token, `%d:%s`, expected `%d`.", parser->current_token->token_type, parser->current_token->value, expected_type);
    exit(1);
}

ast_T* parser_parse_expr(parser_T* parser)
{
    switch (parser->current_token->token_type)
    {
        case T_INTLIT:
        {
            ast_T* ast = init_ast_expr(parser_token_consume(parser, T_INTLIT));
            return ast;
        }
        case T_IDENT:
        {
            ast_T* ast = init_ast_expr(parser_token_consume(parser, T_IDENT));
            return ast;
        }
    }
}

ast_T* parser_parse_exit(parser_T* parser)
{
    parser_token_consume(parser, T_EXIT);
    parser_token_consume(parser, T_LPARAN);
    ast_T* ast = init_ast_node(parser_parse_expr(parser), AST_EXIT);
    parser_token_consume(parser, T_RPARAN);
    parser_token_consume(parser, T_SEMI);
    return ast;
}

ast_T* parser_parse_let(parser_T* parser)
{
    parser_token_consume(parser, T_LET);
    token_T* ident = parser_token_consume(parser, T_IDENT);
    parser_token_consume(parser, T_COLON);
    token_T* data_type = parser_token_consume(parser, T_IDENT);
    parser_token_consume(parser, T_ASSIGN);
    ast_T* expr = parser_parse_expr(parser);
    parser_token_consume(parser, T_SEMI);

    // TODO: [done] - cleanup and hashmap
    ast_T* ast = init_ast_stmnt(expr, ident, AST_LET);

    struct hash_pair* value = hashmap_find(parser->hashmap, data_type->value);
    if (value) ast->data_type = value->value;
    else printf("[WARN]: parser: data_type `%s` is not implemented.\n", data_type->value);

    return ast;
}

// TODO: [done] - create dynamic array to store statements
ast_T* parser_parse(parser_T* parser)
{
    ast_T* stmnt = init_ast_list(AST_STATEMENT);
    while (parser->current_token->token_type != T_EOF)
    {
        switch (parser->current_token->token_type)
        {
            case T_EXIT: array_push(stmnt->lst, parser_parse_exit(parser)); break;
            case T_LET: array_push(stmnt->lst, parser_parse_let(parser)); break;
        }
    }

    return stmnt;
}
