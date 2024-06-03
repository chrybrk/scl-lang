#include <ctype.h>
#include <string.h>
#include "include/lexer.h"
#include "include/utils.h"
#include "include/error.h"

token_T* init_token(int token_type, char* value, int ln, int clm)
{
    token_T* token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->token_type = token_type;
    token->intvalue = 0;
    token->ln = ln;
    token->clm = clm;
    token->value = value;
    return token;
}

char* print_token(int token_type)
{
    switch (token_type)
    {
        case T_EXIT: return "exit";
        case T_LET: return "let";
        case T_EXTERN: return "extern";
        case T_IF: return "if";
        case T_ELSE: return "else";
        case T_LPARAN: return "(";
        case T_RPARAN: return ")";
        case T_LBRACE: return "{";
        case T_RBRACE: return "}";
        case T_SEMI: return ";";
        case T_COLON: return ":";
        case T_COMMA: return ",";
        case T_PLUS: return "+";
        case T_MINUS: return "-";
        case T_STAR: return "*";
        case T_FSLASH: return "/";
        case T_MODULO: return "%";
        case T_ASSIGN: return "=";
        case T_INTLIT: return "integer";
        case T_IDENT: return "identifier";
        case T_STRING: return "string";
        case T_EOF: return "eof";
        default: init_error(E_FAILED, writef("[ERROR]: token: illegal token to print, `%d`\n", token_type));
    }

    return NULL;
}

lexer_T* init_lexer(char* source_file, char* pathname)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->pathname = pathname;
    lexer->source_file = source_file;
    lexer->len = strlen(source_file);
    lexer->index = 0;
    lexer->current_char = lexer->source_file[lexer->index];
    lexer->ln = 1;
    lexer->clm = 1;
    lexer->hashmap = init_hashmap(DEF_SIZE);

    // declare keywords
    hashmap_insert(lexer->hashmap, "exit", T_EXIT);
    hashmap_insert(lexer->hashmap, "let", T_LET);
    hashmap_insert(lexer->hashmap, "extern", T_EXTERN);
    hashmap_insert(lexer->hashmap, "if", T_IF);
    hashmap_insert(lexer->hashmap, "else", T_ELSE);

    return lexer;
}

// it will advance the lexer
void lexer_advance(lexer_T* lexer)
{
    if (lexer->index < lexer->len)
    {
        lexer->clm++;
        lexer->index++, lexer->current_char = lexer->source_file[lexer->index];
    }
    else lexer->current_char = '\0';
}

// it will remove whitespace
// it will also change ln, clm variable.
void lexer_skip_whitespace(lexer_T* lexer)
{
    while (
            lexer->current_char == ' '  ||
            lexer->current_char == '\n' ||
            lexer->current_char == '\t' ||
            lexer->current_char == 10 ||
            lexer->current_char == 32
           )
    {
        if (lexer->current_char == '\n')
        {
            lexer->ln++;
            lexer->clm = 1;
        }
        lexer_advance(lexer);
    }
}

// it collects integer value.
token_T* lexer_collect_integer(lexer_T* lexer)
{
    char* string = calloc(1, sizeof(char) * (sizeof(unsigned long int) * 256));
    ssize_t index = 0;

    while (isdigit(lexer->current_char))
    {
        string[index] = lexer->current_char;
        index++;
        lexer_advance(lexer);
    }

    string[index] = '\0';

    token_T* token = init_token(T_INTLIT, NULL, lexer->ln, lexer->clm);
    token->intvalue = atoi(string);

    return token;
}

// it collects identifier
// -> it can be keyword
token_T* lexer_collect_identifier(lexer_T* lexer)
{
    char* string = calloc(1, sizeof(char));
    ssize_t index = 0;

    while (isalnum(lexer->current_char) || lexer->current_char == '_')
    {
        string[index] = lexer->current_char;
        index++;
        string = realloc(string, sizeof(char) * index);
        lexer_advance(lexer);
    }

    string[index] = '\0';
    token_T* token;

    struct hash_pair* hash = hashmap_find(lexer->hashmap, string);
    if (hash) token = init_token(hash->value, string, lexer->ln, lexer->clm);
    else token = init_token(T_IDENT, string, lexer->ln, lexer->clm);

    return token;
}

// it will generate token from current character.
token_T* lexer_create_current_token(lexer_T* lexer, int token_type)
{
    char string[2];
    string[0] = lexer->current_char;
    string[1] = '\0';
    token_T* token = init_token(token_type, string, lexer->ln, lexer->clm);
    lexer_advance(lexer);
    return token;
}

// it will generate string token from current index to nth index.
token_T* lexer_create_string_token(lexer_T* lexer)
{
    lexer_advance(lexer);

    char* string = calloc(1, sizeof(char));
    ssize_t index = 0;

    while (lexer->current_char && lexer->current_char != '"')
    {
        string[index] = lexer->current_char;
        index++;
        string = realloc(string, sizeof(char) * index);
        lexer_advance(lexer);
    }

    string[index] = '\0';
    token_T* token = init_token(T_STRING, string, lexer->ln, lexer->clm);

    lexer_advance(lexer);

    return token;
}

token_T* next_token(lexer_T* lexer)
{
    while (lexer->current_char)
    {
        lexer_skip_whitespace(lexer);

        if (isdigit(lexer->current_char)) return lexer_collect_integer(lexer);
        if (isalpha(lexer->current_char) || lexer->current_char == '_') return lexer_collect_identifier(lexer);

        if (!lexer->current_char) return init_token(T_EOF, NULL, lexer->ln, lexer->clm);
        switch (lexer->current_char)
        {
            case '"': return lexer_create_string_token(lexer);
            case '(': return lexer_create_current_token(lexer, T_LPARAN);
            case ')': return lexer_create_current_token(lexer, T_RPARAN);
            case '{': return lexer_create_current_token(lexer, T_LBRACE);
            case '}': return lexer_create_current_token(lexer, T_RBRACE);
            case ';': return lexer_create_current_token(lexer, T_SEMI);
            case ':': return lexer_create_current_token(lexer, T_COLON);
            case ',': return lexer_create_current_token(lexer, T_COMMA);
            case '=': return lexer_create_current_token(lexer, T_ASSIGN);
            case '+': return lexer_create_current_token(lexer, T_PLUS);
            case '-': return lexer_create_current_token(lexer, T_MINUS);
            case '*': return lexer_create_current_token(lexer, T_STAR);
            case '/': return lexer_create_current_token(lexer, T_FSLASH);
            case '%': return lexer_create_current_token(lexer, T_MODULO);
        }
    }

    return init_token(T_EOF, NULL, lexer->ln, lexer->clm);
}
