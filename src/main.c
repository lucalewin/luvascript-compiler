// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility functions
#include <options.h>
#include <scope_impl.h>
#include <util/cmd.h>
#include <util/util.h>
#include <util/arraylist.h>
#include <util/file.h>
#include <logging/debug.h>
#include <logging/logger.h>

// types
#include <types/package.h>
#include <types/ast.h>

// core library functions
#include <lexer.h>
#include <parser.h>
#include <compiler.h>

int main(int argc, char **argv) {
	// validate command line arguments
	if (argc < 2) {
		log_error("no input file specified\n");
		log_error("type 'lvc -h' for help\n");
		return -1;
	}

	// parse commandline arguments
	CommandlineOptions *options = parse_commandline_arguments(argc, argv);

	if (options->source_files->size == 0) {
		log_error("no source files specified\n");
		log_error("type 'lvc -h' for help\n");
		return -1;
	}

	// compile the source files
	AST *ast = calloc(1, sizeof(AST));
	ast->packages = arraylist_create();
	ast->global_scope = NULL;

	for (size_t i = 0; i < options->source_files->size; i++) {
		char *source_file_name = arraylist_get(options->source_files, i);
		char *source_code = read_file(source_file_name);
		
		// log_debug("parsing source file %s\n", arraylist_get(options->source_files, i));

		ArrayList *tokens = tokenize(source_code, source_file_name);
		if (tokens == NULL) {
			// free allocated memory
			// `source_file_name` does not need to be freed because it comes from the commandline
			free(source_code);
			ast_free(ast);
			return -1;
		}
		
		Package *package = parse(tokens, source_file_name);
		if (package == NULL) {
			// free allocated memory
			// `source_file_name` does not need to be freed because it comes from the commandline
			free(source_code);
			ast_free(ast);
			return -1;
		}

		arraylist_add(ast->packages, package);

		// log_debug("parsing complete for source file %s\n", source_file_name);

		free(source_code);
		for (size_t i = 0; i < tokens->size; i++)
			token_free(arraylist_get(tokens, i));
		arraylist_free(tokens);
	}

	// if packages have the same name, merge them
	for (size_t i = 0; i < ast->packages->size; i++) {
		Package *package = arraylist_get(ast->packages, i);
		for (size_t j = i + 1; j < ast->packages->size; j++) {
			Package *other_package = arraylist_get(ast->packages, j);
			if (strcmp(package->name, other_package->name) == 0) {
				// log_debug("merging packages '%s' and '%s'\n", package->name, other_package->name);
				package_merge(package, other_package);
				arraylist_remove_at_index(ast->packages, j);
				j--;
			}
		}
	}

	if (scope_evaluate_ast(ast) != 0) {
		// free allocated memory
		options_free(options);
		ast_free(ast);
		return -1;
	}

	// generate assembly code
	char *assembly_code = compile(ast, options);

	if (options->output_file_name == NULL) {
		log_error("output_file_name is not specified\n");
		// free allocated memory
		options_free(options);
		ast_free(ast);
		return -1;
	}

	// create filename of the assembly file
	char *asm_file_name = stradd(options->output_file_name, ".asm");

	// write assembly code to file
	write_file(asm_file_name, assembly_code);

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
		exec(linker, "-g", obj_file, "-o", options->output_file_name, NULL);
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

	log_info("successfully compiled to '%s'\n", options->output_file_name);

	return 0;
}
