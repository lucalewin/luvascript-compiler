#include <stdio.h>
#include <stdlib.h>

#include <util.h>
#include <arraylist.h>
#include <lexer.h>
#include <parser.h>
#include <logger.h>

int main(int argc, char **argv) {
    // validate command line arguments
    if (argc < 2) {
        print_debug("invalid argument length: %d\n", argc - 1);
        print_error("no input file specified\n");
        print_error("type 'lvc -h' for help\n");
        return -1;
    }

    print_info("reading file contents from '%s'\n", argv[1]);
    
    char* file_contents = read_file(argv[1]);           // read file
    
    // 
    ArrayList *tokens = lexer_start(file_contents);

    exprParserStart(tokens);
    arraylist_free(tokens);

    // int last_index = str_last_index_of(argv[1], '.');   //
    // char *file_base_name = malloc(last_index);          // get filepath without extension
    // substring(argv[1], file_base_name, last_index);     //
    // free(file_base_name);

    return 0;
}
