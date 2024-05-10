#include "include/parser.h"

ast_T* init_ast_exit(ast_T* expr_node)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = AST_EXIT;
    ast->expr_node = expr_node;
    ast->token = NULL;
    return ast;
}

ast_T* init_ast_expr(token_T* token)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = AST_EXPR;
    ast->expr_node = NULL;
    ast->token = token;
    return ast;
}

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = next_token(parser->lexer);
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
    }
}

ast_T* parser_parse_exit(parser_T* parser)
{
    parser_token_consume(parser, T_EXIT);
    parser_token_consume(parser, T_LPARAN);
    ast_T* ast = init_ast_exit(parser_parse_expr(parser));
    parser_token_consume(parser, T_RPARAN);
    parser_token_consume(parser, T_SEMI);
    return ast;
}

ast_T* parser_parse(parser_T* parser)
{
    switch (parser->current_token->token_type)
    {
        case T_EXIT: return parser_parse_exit(parser);
    }
}
