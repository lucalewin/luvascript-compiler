#include <stdio.h>
#include <stdlib.h>

#include <util.h>
#include <logger.h>
#include <arraylist.h>
#include <lexer.h>
#include <parser.h>
#include <compiler.h>

#define DEFAULT_BINARY_NAME "a.out"

int main(int argc, char **argv) {
    // validate command line arguments
    if (argc < 2) {
        log_debug("ERROR: invalid argument length: %d\n", argc - 1);
        log_error("no input file specified\n");
        log_error("type 'lvc -h' for help\n");
        return -1;
    }

    // TODO: parse commandline arguments

    log_debug("reading file contents from '%s'\n", argv[1]);
    char* file_contents = read_file(argv[1]);
    
    ArrayList *tokens = tokenize(file_contents);

    parse_tokens(tokens);
    arraylist_free(tokens);

    // int last_index = str_last_index_of(argv[1], '.');   //
    // char *file_base_name = malloc(last_index);          // get filepath without extension
    // substring(argv[1], file_base_name, last_index);     //
    // free(file_base_name);

    return 0;
}
