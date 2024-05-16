#include <stdio.h>
#include <stdlib.h>
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/gen.h"

// TODO: [done] - update grammar rule, handling identifier in expr.
// FIXME: [done] - unable to generate proper assembly.
// TODO: update grammar rule, handling math expr.
// TODO: able to make use of, `extrn` keyword in fasm.

int main(int argc, char** argv)
{
    // check if we have input file
    if (argc < 2) printf("scl: no input file."), exit(1);

    // read input file
    char* source = read_file(argv[1]);

    lexer_T* lexer = init_lexer(source);
    parser_T* parser = init_parser(lexer);
    ast_T* root = parser_parse(parser);

    gen_T* gen = init_gen("bin/main.s");
    gen_program(gen, root);

    system("fasm bin/main.s");
    system("gcc -no-pie -nostdlib -lc bin/main.o -o bin/main");

    return 0;
}
