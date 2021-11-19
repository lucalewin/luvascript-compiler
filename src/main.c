// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility functions
#include <options.h>
#include <util/cmd.h>
#include <util/util.h>
#include <util/arraylist.h>
#include <util/file.h>
#include <logging/debug.h>
#include <logging/logger.h>

// core library functions
#include <lexer.h>
#include <parser.h>
#include <compiler.h>

// default filename of the generated binary file if no output filename is specified
#define DEFAULT_BINARY_NAME "a.out"
#define VERSION "0.1.0-alpha"

void print_help();

int main(int argc, char **argv) {
	// validate command line arguments
	if (argc < 2) {
		log_error("no input file specified\n");
		log_error("type 'lvc -h' for help\n");
		return -1;
	}

	// parse commandline arguments
	CommandlineOptions *options = parse_commandline_arguments(argc, argv);

	// check if a scource file is specified
	if (options->input_file_name == NULL) {
		log_error("no input file specified\n");
		log_error("type 'lvc -h' for help\n");
		return -1;
	}

	// read code from source file
	char* file_contents = read_file(options->input_file_name);
	if (file_contents == NULL) {
		options_free(options);
		exit(1);
	}

	// tokenize code
	ArrayList *tokens = tokenize(file_contents);
	free(file_contents);

	// parse tokens to an ast
	AST *root = parse(tokens);
	for (size_t i = 0; i < tokens->size; i++) {
		Token *token = arraylist_get(tokens, i);
		token_free(token);
	}
	arraylist_free(tokens);

	// compile ast to x86_64 assembly
	char *asm_code = compile_to_x86_64_assembly(root);
	ast_free(root);

	// create filename of the assembly file
	char *asm_file_name = stradd(options->input_file_name, ".asm");

	// create / write asm file
	write_file(asm_file_name, asm_code);

	// free allocated memory for asm_code
	free(asm_code);

	char *assembler = "nasm";
	char *file_format = "-f elf64";
	char *linker = "ld";

	// compile asm file
	if (options->link) {
		// `-c` compiler flag is specified
		char *obj_file = calloc(1, sizeof(char));
		obj_file = straddall(obj_file, asm_file_name, ".o", NULL);
		// compile to object file
		exec(assembler, file_format, asm_file_name, "-o", obj_file, NULL);
		// link to binary file
		exec(linker, "-g", obj_file, options->output_file_name, NULL);
		// remove generated assembly file
		if (!options->generate_assembly) {
			exec("rm", asm_file_name, NULL);
		}

		// remove object file
		exec("rm", obj_file, NULL);

		// free allocated memory for char*
		free(obj_file);
	} else {
		// compile to object file
		exec(assembler, file_format, asm_file_name, "-o", options->output_file_name, NULL);

		// remove generated assembly file
		if (!options->generate_assembly) {
			exec("rm", asm_file_name, NULL);
		}
	}

	free(asm_file_name);

	log_info("successfully compiled '%s'\n", options->input_file_name);

	return 0;
}

/**
 * @brief parses the command line arguments
 * 
 * @param argc the number of arguments
 * @param argv the arguments
 * @return CommandlineOptions* 
 */
CommandlineOptions *parse_commandline_arguments(int argc, char *argv[]) {
	CommandlineOptions *options = calloc(1, sizeof(CommandlineOptions));

	// assign default values to options
	options->input_file_name = NULL;
	options->output_file_name = DEFAULT_BINARY_NAME;
	options->link = 1; // true
	options->generate_assembly = 0; // false

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) { // -c
			options->link = 0; // don't link
		} else if (strcmp(argv[i], "-S") == 0) {
			options->generate_assembly = 1; // generate assembly file
		} else if (strcmp(argv[i], "-o") == 0) { // -o
			if (i + 1 < argc) {
				options->output_file_name = calloc(strlen(argv[i + 1]), sizeof(char));
				strcpy(options->output_file_name, argv[i + 1]);
				i++;
			}
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { // -h, --help
			print_help();
			exit(0);
		} else if (strcmp(argv[i], "--version") == 0) { // --version
			printf("lvc version %s\n", VERSION);
			exit(0);
		} else {
			if (options->input_file_name != NULL) {
				free(options->input_file_name);
			}
			options->input_file_name = calloc(strlen(argv[i]), sizeof(char));
			strcpy(options->input_file_name, argv[i]);
		}
	}

	return options;
}

/**
 * @brief 
 * TODO: add help for compiler flags
 */
void print_help() {
	printf(YELLOW "usage" RESET ": lvc <inputfile>\n");
}