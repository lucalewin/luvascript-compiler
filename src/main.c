#include <stdio.h>
#include <stdlib.h>

#include <util.h>
#include <arraylist.h>
#include <lexer.h>
#include <parser.h>
#include <compiler.h>
#include <ast.h>

int main(int argc, char **argv) {
    // validate command line arguments
    if (argc < 2) {
        printf("invalid argument length: %d\n", argc);
        return -1;
    }

    int last_index = str_last_index_of(argv[1], '.');   //
    char *file_base_name = malloc(last_index);          // get filepath without extension
    substring(argv[1], file_base_name, last_index);     //

    
    char* file_contents = read_file(argv[1]);           // read file
    
    ArrayList *list = lexer_start(file_contents);       // get list of tokens from code
    free(file_contents);                                // free file contents -> no longer needed
    AST *ast = parser_create_ast(list);                 // create AST from tokens
    arraylist_free(list);                               // free tokenlist

    char *asm = convert_ast_to_x86_64_assembly(ast);

    printf("%s\n", asm);

    // compile(file_base_name);

    free(file_base_name);

    return 0;
}
