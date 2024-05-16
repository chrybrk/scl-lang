#include "include/lexer.h"
#include <ctype.h>
#include <string.h>

token_T* init_token(int token_type, char* value)
{
    token_T* token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->token_type = token_type;
    token->value = value;
    token->intvalue = 0;
    return token;
}

lexer_T* init_lexer(char* source_file)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->source_file = source_file;
    lexer->len = strlen(source_file);
    lexer->index = 0;
    lexer->current_char = lexer->source_file[lexer->index];
    lexer->hashmap = init_hashmap(DEF_SIZE);

    // declare keywords
    hashmap_insert(lexer->hashmap, "exit", T_EXIT);
    hashmap_insert(lexer->hashmap, "let", T_LET);
    hashmap_insert(lexer->hashmap, "extern", T_EXTERN);

    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    lexer->current_char = '\0';
    if (lexer->index < lexer->len)
        lexer->index++, lexer->current_char = lexer->source_file[lexer->index];
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while (
            lexer->current_char == ' '  ||
            lexer->current_char == '\n' ||
            lexer->current_char == '\t' ||
            lexer->current_char == 10 ||
            lexer->current_char == 32
           ) lexer_advance(lexer);
}

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

    token_T* token = init_token(T_INTLIT, NULL);
    token->intvalue = atoi(string);

    return token;
}

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
    if (hash) token = init_token(hash->value, string);
    else token = init_token(T_IDENT, string);

    return token;
}

token_T* lexer_create_current_token(lexer_T* lexer, int token_type)
{
    char string[2];
    string[0] = lexer->current_char;
    string[1] = '\0';
    token_T* token = init_token(token_type, string);
    lexer_advance(lexer);
    return token;
}

token_T* next_token(lexer_T* lexer)
{
    /*
     * skip whitespaces
     * check for integer
     * check for identifier
     * check for
     *      lparan
     *      rparan
     *      semi
    */

    while (lexer->current_char)
    {
        lexer_skip_whitespace(lexer);

        if (isdigit(lexer->current_char)) return lexer_collect_integer(lexer);
        if (isalpha(lexer->current_char)) return lexer_collect_identifier(lexer);

        if (!lexer->current_char) return init_token(T_EOF, NULL);
        switch (lexer->current_char)
        {
            case '(': return lexer_create_current_token(lexer, T_LPARAN);
            case ')': return lexer_create_current_token(lexer, T_RPARAN);
            case ';': return lexer_create_current_token(lexer, T_SEMI);
            case ':': return lexer_create_current_token(lexer, T_COLON);
            case '=': return lexer_create_current_token(lexer, T_ASSIGN);
        }
    }

    return init_token(T_EOF, NULL);
}
