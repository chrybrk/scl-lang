#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/utils.h"
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/gen.h"
#include "include/error.h"

void scl_help()
{
    printf("scl: help\n\
            \tscl <inputfile> <arguments>\n\
            \t-o: output\n\
            \t-a: generate assembly\n\
            \t-O: generate object\n\
            \t-r: compile and run\n\
            \t-l: linker input, -l<libs>\n\
            \t-L: linker libs path, -L<path>\n\
            \t-I: linker libs include path, -I<path>\n\
            \t-C: compile `.c` file and link\n\
            \t-E: lazy evaluation, for variable, function, externs and loops\n"
    );
}

/*
 * GLOBALS
 * DECLARATIONS
*/
char* scl_current_file;
char* scl_out_file;
char* scl_c_file;
bool  scl_flags[3];
array_T* scl_in_file;
array_T* scl_libs;
array_T* scl_libs_path;
array_T* scl_libs_include_path;
array_T* scl_error_list;
array_T* scl_python_readlines;

int main(int argc, char** argv)
{
    if (argc < 2) scl_help(), exit(1);
    argv++;

    scl_in_file = init_array(sizeof(char*));
    scl_libs = init_array(sizeof(char*));
    scl_libs_path = init_array(sizeof(char*));
    scl_libs_include_path = init_array(sizeof(char*));

    while (argv[0])
    {
        char* e = argv[0];
        switch (e[0])
        {
            case '-':
            {
                switch ((++e)[0])
                {
                    case 'o':
                    {
                        scl_out_file = alloc_str((++argv)[0], scl_out_file);
                        if (!scl_out_file || scl_out_file[0] == '-') printf("[scl_err]: expected <output_file> after -o.\n"), exit(1);
                        argv++;
                        continue;
                    }
                    case 'a': scl_flags[0] = true; break;
                    case 'O': scl_flags[1] = true; break;
                    case 'r': scl_flags[2] = true; break;
                    case 'l':
                    case 'L':
                    case 'I':
                    {
                        char t_e = e[0];
                        e++;

                        char* lib = NULL;
                        while (e[0])
                        {
                            char s[2] = { e[0], '\0' };
                            lib = alloc_str(s, lib);
                            e++;
                        }

                        if (t_e == 'l') array_push(scl_libs, lib);
                        else if (t_e == 'L') array_push(scl_libs_path, lib);
                        else if (t_e == 'I') array_push(scl_libs_include_path, lib);

                        break;
                    }
                    case 'C':
                    {
                        scl_c_file = alloc_str((++argv)[0], scl_c_file);
                        if (!scl_c_file || scl_c_file[0] == '-') printf("[scl_err]: expected <c_file> after -C.\n"), exit(1);
                        argv++;
                        continue;
                    }
                }
                break;
            }
            default:
            {
                char* f = NULL;
                f = alloc_str(argv[0], f);
                array_push(scl_in_file, f);
            }
        }

        argv++;
    }

    if (scl_in_file->index == 0) printf("[scl_err]: no input file.\n");
    if (!scl_out_file) scl_out_file = alloc_str("main", scl_out_file);

    ast_T* root = init_ast_list(AST_STATEMENT);

    for (ssize_t i = 0; i < scl_in_file->index; i++)
    {
        char* path = (char*)scl_in_file->buffer[i];
        char* readf = read_file(path);

        if (scl_python_readlines) free(scl_python_readlines), scl_python_readlines = NULL;
        if (!scl_python_readlines) scl_python_readlines = init_array(sizeof(char*));

        char* line = calloc(1, sizeof(char));
        for (size_t i = 0; i < strlen(readf)+1; i++)
        {
            if (readf[i] != '\n' && readf[i] != '\0')
            {
                line = strcat(line, writef("%c\0", readf[i]));
            }
            else
            {
                array_push(scl_python_readlines, line);
                line = calloc(1, sizeof(char));
            }
        }

        scl_current_file = path;
        lexer_T* lexer = init_lexer(readf, path);
        parser_T* parser = init_parser(lexer);
        ast_T* node = parser_parse(parser);
        for (ssize_t i = 0; i < node->lst->index; i++)
            array_push(root->lst, node->lst->buffer[i]);
    }

    gen_T* gen = init_gen(writef("%s.s", scl_out_file));
    gen_program(gen, root);

    error_flush();

    char* libs_for_gcc = NULL;
    for (ssize_t i = 0; i < scl_libs->index; i++)
        libs_for_gcc = alloc_str(writef("-l%s ", scl_libs->buffer[i]), libs_for_gcc);

    char* libs_path_for_gcc = NULL;
    for (ssize_t i = 0; i < scl_libs_path->index; i++)
        libs_path_for_gcc = alloc_str(writef("-L%s ", scl_libs_path->buffer[i]), libs_path_for_gcc);

    char* libs_include_path_for_gcc = NULL;
    for (ssize_t i = 0; i < scl_libs_include_path->index; i++)
        libs_include_path_for_gcc = alloc_str(writef("-L%s ", scl_libs_include_path->buffer[i]), libs_include_path_for_gcc);

    system(writef("fasm %s.s", scl_out_file));
    system(writef("gcc -nostdlib -nostartfiles -nolibc -nostdinc -no-pie %s %s %s %s %s.o -o %s",
                libs_for_gcc ? libs_for_gcc : "",
                libs_path_for_gcc ? libs_path_for_gcc : "",
                libs_include_path_for_gcc ? libs_include_path_for_gcc : "",
                scl_c_file ? scl_c_file : "",
                scl_out_file,
                scl_out_file
    ));

    if (!scl_flags[0]) system(writef("rm %s.s", scl_out_file));
    if (!scl_flags[1]) system(writef("rm %s.o", scl_out_file));
    if (scl_flags[2]) system(writef("./%s", scl_out_file));

    return 0;
}
