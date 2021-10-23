// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility functions
#include <util/util.h>
#include <util/arraylist.h>
#include <util/file.h>
#include <logging/debug.h>
#include <logging/logger.h>

// core library functions
#include <lexer.h>
#include <parser.h>
#include <compiler.h>

#define DEFAULT_BINARY_NAME "a.out"

void print_help();

int main(int argc, char **argv) {
	// validate command line arguments
	if (argc < 2) {
		log_debug("ERROR: invalid argument length: %d\n", argc - 1);
		log_error("no input file specified\n");
		log_error("type 'lvc -h' for help\n");
		return -1;
	}

	// TODO: parse commandline arguments

	if (strcmp (argv[1], "-h") == 0) {
		print_help();
		return 0;
	}

	// log_debug("reading file contents from '%s'\n", argv[1]);
	// read code
	char* file_contents = read_file(argv[1]);
	if (!file_contents) {
		exit(1);
	}
    
	// tokenize code
	ArrayList *tokens = tokenize(file_contents);
	// parse tokens to an ast
	AST *root = parse(tokens);
	arraylist_free(tokens);

	// print ast
	// log_debug("{");
	// print_statement(root->statement);
	// printf("}\n");

	// compile ast to x86_64 assembly
	char *asm_code = compile_to_x86_64_assembly(root);
	free(root);

	// create filename of the assembly file
	char *asm_file_name = stradd(argv[1], ".asm");

	// create / write asm file
	write_file(asm_file_name, asm_code);

	// compile asm file
	compile_asm_file(asm_file_name, DEFAULT_BINARY_NAME);
	free(asm_file_name);

	log_info("successfully compiled '%s'\n", argv[1]);

	return 0;
}

void print_help() {
	printf(YELLOW "usage" RESET ": lvc <inputfile>\n");
}