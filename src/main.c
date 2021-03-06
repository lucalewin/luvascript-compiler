#include <stdio.h>
#include <stdlib.h>

#include "include/util.h"
#include "include/arraylist.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/compiler.h"
#include "include/ast.h"

int main(int argc, char **argv) {
    // validate command line arguments
    if (argc < 2) {
        printf("invalid argument length: %d\n", argc);
        return -1;
    }

    int last_index = str_last_index_of(argv[1], '.');
    char *file_base_name = malloc(last_index);
    substring(argv[1], file_base_name, last_index);

    
    char* file_contents = read_file(argv[1]);           // read file
    
    ArrayList *list = lexer_start(file_contents);       // get list of tokens from code
    free(file_contents);                                // free file contents -> no longer needed
    AST *ast = parser_create_ast(list);                 // create AST from tokens
    arraylist_free(list);                               // free tokenlist

    // ---------------------------------

    parser_generate_assembly_from_expr(file_base_name, NULL);

    // ---------------------------------

    compile(file_base_name);

    /* run generated assembly */
    // system(file_base_name);

    free(file_base_name);

    return 0;
}
