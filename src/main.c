#include <stdio.h>
#include <stdlib.h>

#include <util.h>
#include <debug.h>
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
    // read code
    char* file_contents = read_file(argv[1]);
    
    // tokenize code
    ArrayList *tokens = tokenize(file_contents);
    // parse tokens to an ast
    AST *root = parse(tokens);
    arraylist_free(tokens);

    // print ast
    log_debug("{");
    print_statement(root->statement);
    printf("}\n");

    // parse ast to x86_64 assembly 
    char *asm_code = parse_ast_to_x86_64_asm(root);
    free(root);

    // create filename of the assembly file
    char *asm_file_name = calloc(strlen(argv[1]), sizeof (char));
    stradd(asm_file_name, argv[1]);
    stradd(asm_file_name, ".asm");

    // create / write asm file
    write_file(asm_file_name, asm_code);
    
    // if (asm != NULL) {
    //     free(asm);
    // }

    // compile asm file
    compile_asm(asm_file_name, DEFAULT_BINARY_NAME);
    free(asm_file_name);

    log_info("successfully compiled '%s'\n", argv[1]);

    return 0;
}
