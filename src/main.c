#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <options.h>
#include <scope_impl.h>
#include <util/util.h>
#include <util/arraylist.h>
#include <util/file.h>
#include <logging/logger.h>

#include <types/package.h>
#include <types/ast.h>

#include <lexer.h>
#include <parser.h>
#include <generator.h>
#include <compiler.h>

// directory of the globaly installed libraries for luvascript
#define GLOBAL_LIBRARY_DIR "/usr/lib/luva"

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
	AST *ast = ast_new();

	for (size_t i = 0; i < options->source_files->size; i++) {
		char *source_file_name = arraylist_get(options->source_files, i);

		if (!file_exists(source_file_name)) {
			log_error("source file '%s' does not exist\n", source_file_name);
			// free allocated memory
			for (size_t j = 0; j < ast->packages->size; j++) {
				Package *package = arraylist_get(ast->packages, j);
				package_free(package);
			}
			ast_free(ast);
			options_free(options);
			return -1;
		}

		char *source_code = read_file(source_file_name);
		
		if (source_code == NULL) {
			printf("    " RED "error: " RESET "cannot read file '%s'\n", source_file_name);
			continue;
		}

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

	// load global libraries
	ArrayList *global_libraries_dir_module_dirs = list_dirs(GLOBAL_LIBRARY_DIR);
	// print filename of global libraries
	for (size_t i = 2; i < global_libraries_dir_module_dirs->size; i++) {
		char *module_dir = arraylist_get(global_libraries_dir_module_dirs, i);
		log_debug("loading global module: %s\n", module_dir);

		// load all files in the module directory
		ArrayList *module_dir_files = list_files(module_dir);
		// extract all header files
		for (size_t j = 0; j < module_dir_files->size; j++) {
			char *file_name = arraylist_get(module_dir_files, j);
			if (strcmp(get_filename_extension(file_name), "lvh") == 0) {
				char *header_file_name = concat(module_dir, "/", file_name);
				char *header_file_source_code = read_file(header_file_name);
				ArrayList *header_file_tokens = tokenize(header_file_source_code);
				// Package *header_file_package = parse(header_file_tokens);
				AST *module_ast = parse_header(header_file_tokens);
				// arraylist_add(ast->packages, header_file_package);
				free(header_file_source_code);
				for (size_t i = 0; i < header_file_tokens->size; i++)
					token_free(arraylist_get(header_file_tokens, i));
				arraylist_free(header_file_tokens);
				free(header_file_name);
			}
		}
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
	char *assembly_code = generate_assembly(ast, options); //compile(ast, options);

	if (options->is_library) {
		// generate header for library
		char *header_code = generate_header(ast, options);
		// write header to file
		char *header_file_name = malloc(strlen(options->output_file_name) + 5);
		strcpy(header_file_name, options->output_file_name);
		strcat(header_file_name, ".lvh");
		write_file(header_file_name, header_code);
		free(header_file_name);
		free(header_code);
	}

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

	compile(asm_file_name, options);

	free(asm_file_name);

	log_info("successfully compiled to '%s'\n", options->output_file_name);

	return 0;
}
