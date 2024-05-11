#include <stdio.h>
#include <stdlib.h>
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/gen.h"

// TODO: update grammar rule, handling identifier in expr.

int main(int argc, char** argv)
{
    // check if we have input file
    if (argc < 2) printf("scl: no input file."), exit(1);

    // read input file
    char* source = read_file(argv[1]);

    lexer_T* lexer = init_lexer(source);
    parser_T* parser = init_parser(lexer);
    ast_T* root = parser_parse(parser);

    gen_preamble("bin/main.s");
    gen_program(root);
    gen_postamble();

    system("nasm -felf64 bin/main.s -o bin/main.o");
    system("ld bin/main.o -o bin/main");

    return 0;
}
