#include <string.h>
#include "include/global.h"
#include "include/parser.h"
#include "include/utils.h"
#include "include/error.h"

ast_T* init_ast(int type)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->lst = NULL;
    ast->token = NULL;
    return ast;
}

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

ast_T* init_ast_with_token(int type, token_T* token)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->node = NULL;
    ast->lst = NULL;
    ast->token = token;
    return ast;
}

ast_T* init_ast_with_two_nodes(ast_T* left_node, ast_T* node, int type)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = type;
    ast->left_node = left_node;
    ast->node = node;
    ast->lst = NULL;
    ast->token = NULL;
    return ast;
}

ast_T* init_binOP(ast_T* left, ast_T* right, int op)
{
    ast_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->ast_type = AST_BINOP;
    ast->left_node = left;
    ast->right_node = right;
    ast->op = op;
    ast->node = NULL;
    ast->lst = NULL;
    ast->token = NULL;
    return ast;
}

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = next_token(parser->lexer);
    parser->current_label = 0;
    parser->hashmap = init_hashmap(DEF_SIZE);

    hashmap_insert(parser->hashmap, "void", VOID);
    hashmap_insert(parser->hashmap, "char", CHAR);
    hashmap_insert(parser->hashmap, "string", STRING);
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

    init_error_with_lexer(parser->lexer, E_FAILED, writef("illegal token `%s`, expected `%s`.",
        print_token(parser->current_token->token_type),
        print_token(expected_type)
    ));

    return NULL;
}

token_T* parser_token_peek(parser_T* parser, int offset)
{
    parser_T* parser_copy = calloc(1, sizeof(struct PARSER_STRUCT));
    parser_copy->lexer = parser->lexer, parser_copy->current_token = parser->current_token;

    token_T* token = parser_copy->current_token;
    for (int i = 0; i < offset; i++) parser_token_consume(parser_copy, parser_copy->current_token->token_type);

    return token;
}

ast_T* parser_parse_factor(parser_T* parser)
{
    switch (parser->current_token->token_type)
    {
        case T_LPARAN:
        {
            parser_token_consume(parser, T_LPARAN);
            ast_T* ast = parser_parse_expr(parser);
            parser_token_consume(parser, T_RPARAN);
            
            return ast;
        }

        case T_INTLIT:
        {
            ast_T* ast = init_ast_with_token(AST_EXPR, parser_token_consume(parser, T_INTLIT));
            return ast;
        }
        case T_IDENT:
        {
            token_T* variable = parser_token_consume(parser, T_IDENT);
            if (parser->current_token->token_type == T_ASSIGN)
            {
                parser_token_consume(parser, T_ASSIGN);
                ast_T* expr = parser_parse_expr(parser);
                return init_ast_stmnt(expr, variable, AST_VAR);
            }
            else if (parser->current_token->token_type == T_LPARAN)
            {
                ast_T* ast = init_ast(AST_CALL);
                ast->token = variable;
                ast->lst = init_array(sizeof(struct AST_STRUCT));

                parser_token_consume(parser, T_LPARAN);
                ast_T* expr = parser_parse_expr(parser);
                array_push(ast->lst, expr);
                while (parser->current_token->token_type == T_COMMA)
                {
                    parser_token_consume(parser, T_COMMA);
                    expr = parser_parse_expr(parser);
                    array_push(ast->lst, expr);
                }
                parser_token_consume(parser, T_RPARAN);

                return ast;
            }
            return init_ast_with_token(AST_EXPR, variable);
        }
        case T_STRING:
        {
            ast_T* ast = init_ast_with_token(AST_EXPR, parser_token_consume(parser, T_STRING));
            return ast;
        }
        default:
        {
            init_error_with_lexer(parser->lexer,
                    E_FAILED,
                    writef("illegal token, `%s`.", print_token(parser->current_token->token_type))
            ), error_flush();
            parser_token_consume(parser, parser->current_token->token_type);
        }
    }

    return NULL;
}

ast_T* parser_parse_term(parser_T* parser)
{
    ast_T* left = parser_parse_factor(parser);

    while (parser->current_token->token_type == T_STAR || parser->current_token->token_type == T_FSLASH || parser->current_token->token_type == T_MODULO)
        left = init_binOP(left, parser_parse_factor(parser), parser_token_consume(parser, parser->current_token->token_type)->token_type);

    return left;
}

ast_T* parser_parse_addititve(parser_T* parser)
{
    ast_T* left = parser_parse_term(parser);

    while (parser->current_token->token_type == T_PLUS || parser->current_token->token_type == T_MINUS)
        left = init_binOP(left, parser_parse_term(parser), parser_token_consume(parser, parser->current_token->token_type)->token_type);

    return left;
}

ast_T* parser_parse_expr(parser_T* parser)
{
    ast_T* left = parser_parse_addititve(parser);

    while (
        parser->current_token->token_type == T_GT   ||
        parser->current_token->token_type == T_LT   ||
        parser->current_token->token_type == T_GTE  ||
        parser->current_token->token_type == T_LTE  ||
        parser->current_token->token_type == T_EQ   ||
        parser->current_token->token_type == T_AND  ||
        parser->current_token->token_type == T_OR   ||
        parser->current_token->token_type == T_NOT  ||
        parser->current_token->token_type == T_NEQ
    )
        left = init_binOP(left, parser_parse_addititve(parser), parser_token_consume(parser, parser->current_token->token_type)->token_type);

    return left;
}

ast_T* parser_parse_exit(parser_T* parser)
{
    parser_token_consume(parser, T_EXIT);
    parser_token_consume(parser, T_LPARAN);
    ast_T* ast = init_ast_node(parser_parse_expr(parser), AST_EXIT);
    parser_token_consume(parser, T_RPARAN);
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

    if (!data_type)
        init_error_with_lexer(parser->lexer, E_FAILED, "expected data type, `char, i16, i32, i64, string`."), error_flush();

    if (!expr)
        init_error_with_lexer(parser->lexer, E_FAILED, "expected expr."), error_flush();

    struct hash_pair* value = value = hashmap_find(parser->hashmap, data_type->value);
    if (!value) init_error_with_lexer(parser->lexer, E_FAILED, writef("undefined data type `%s`, expected `char, string, i16, i32, i64`.", data_type->value));

    ast_T* ast = init_ast_stmnt(expr, ident, AST_LET);
    ast->data_type = value->value;

    return ast;
}

ast_T* parser_parse_extern(parser_T* parser)
{
    parser_token_consume(parser, T_EXTERN);
    token_T* data_type = parser_token_consume(parser, T_IDENT);
    token_T* ident = parser_token_consume(parser, T_IDENT);

    if (!data_type || !ident)
        init_error_with_token(parser->current_token, E_FAILED, "illegal behaviour, expected `extern <data_type> <identifier>;`"),
            error_flush();

    ast_T* ast = init_ast_with_token(AST_EXTERN, ident);

    struct hash_pair* value = hashmap_find(parser->hashmap, data_type->value);
    if (value) ast->data_type = value->value;
    else init_error_with_lexer(parser->lexer, E_FAILED, writef("undefined data type `%s`, expected `char, string, i16, i32, i64`.", data_type->value));

    return ast;
}

ast_T* parser_parse_if(parser_T* parser)
{
    parser_token_consume(parser, T_IF);

    return NULL;
}

ast_T* parser_parse_statement(parser_T* parser)
{
    ast_T* ast = NULL;

    switch (parser->current_token->token_type)
    {
        case T_EXIT: ast = parser_parse_exit(parser); break; 
        case T_LET: ast = parser_parse_let(parser); break; 
        case T_EXTERN: ast = parser_parse_extern(parser); break;
        default: ast = parser_parse_expr(parser);
    }

    parser_token_consume(parser, T_SEMI);
    return ast;
}

ast_T* parser_parse_compound(parser_T* parser)
{
    switch (parser->current_token->token_type)
    {
        case T_LBRACE:
        {
            parser_token_consume(parser, T_LBRACE);

            ast_T* stmnt = init_ast_list(AST_STATEMENT);
            stmnt->op = parser->current_label; parser->current_label++;

            while (parser->current_token->token_type != T_EOF && parser->current_token->token_type != T_RBRACE)
            {
                ast_T* ast = parser_parse_compound(parser);

                array_push(stmnt->lst, ast);
            }

            parser_token_consume(parser, T_RBRACE);

            return stmnt;
        }

        case T_IF: return parser_parse_if(parser);
        default: return parser_parse_statement(parser);
    }
}

ast_T* parser_parse(parser_T* parser)
{
    ast_T* stmnt = init_ast_list(AST_STATEMENT);
    stmnt->op = parser->current_label; parser->current_label++;

    while (parser->current_token->token_type != T_EOF)
        array_push(stmnt->lst, parser_parse_compound(parser));

    return stmnt;
}
