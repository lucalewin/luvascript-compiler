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
#include <types/module.h>

#include <lexer.h>
#include <parser.h>
#include <typechecker.h>
#include <generator.h>
#include <compiler.h>

// directory of the globaly installed libraries for luvascript
#define GLOBAL_LIBRARY_DIR "/usr/lib/luva/"

int main(int argc, char **argv)
{
	// parse commandline arguments
	CommandlineOptions *options = parse_commandline_arguments(argc, argv);

	// check if the user specified source files
	if (options->source_files->size == 0) {
		log_error("no source files specified\n");
		log_error("type 'lvc -h' for help\n");
		options_free(options);
		return -1;
	}

	// compile the source files
	AST *ast = ast_new();
	for (size_t i = 0; i < options->source_files->size; i++) {
		char *source_file_name = arraylist_get(options->source_files, i);

		if (!file_exists(source_file_name)) {
			// log_error("source file '%s' does not exist\n", source_file_name);
			printf("    " RED "error: " RESET "source file '%s' does not exist\n", source_file_name);
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
		package->file_path = to_absolute_path(source_file_name);

		arraylist_add(ast->packages, package);

		// log_debug("parsing complete for source file %s\n", source_file_name);

		free(source_code);
		for (size_t i = 0; i < tokens->size; i++)
			token_free(arraylist_get(tokens, i));
		arraylist_free(tokens);
	}

	// load global modules
	// ArrayList *modules = arraylist_create();
	// if (!options->nostdlib)
	// {
	// 	// struct module *std_module = module_load(GLOBAL_LIBRARY_DIR "std");
	// 	// arraylist_add(modules, std_module);
	// }

	// evaluate the scopes of the packages
	if (scope_evaluate_ast(options, ast) != 0) {
		// free allocated memory
		options_free(options);
		ast_free(ast);
		return -1;
	}

	// check types
	if (check_types(ast) == 0) {
		// free allocated memory
		log_debug("check_types failed\n");
		options_free(options);
		ast_free(ast);
		return -1;
	}

	// generate assembly code
	char *assembly_code = generate_assembly(ast, options);
	if (assembly_code == NULL) {
		// free allocated memory
		options_free(options);
		ast_free(ast);
		return -1;
	}

	// compile assembly code
	compile(assembly_code, options);

	return 0;
}
