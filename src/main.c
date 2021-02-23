#include <stdio.h>
#include <stdlib.h>

#include "include/util.h"
#include "include/lexer.h"
#include "include/parser.h"

int main(int argc, char **argv) {
    // validate command line arguments
    if (argc < 2) {
        printf("invalid argument length: %d\n", argc);
        return -1;
    }

    // read file
    char* file_contents = read_file(argv[1]);

    // get list of tokens from code
    TokenList *list = lexer_start(file_contents);

    // create AST from tokens
    parser_create_ast(list);

    // free file contents -> no longer needed
    free(file_contents);
    
    // free tokenlist
    tokenlist_free(list);
    return 0;
}