#include <types/ast.h>

#include <stdio.h>

#include <scope_impl.h>
#include <util/arraylist.h>
#include <util/util.h>
#include <util/file.h>
#include <types/package.h>
#include <options.h>

AST *ast_new() {
	AST *ast = calloc(1, sizeof(AST));
	ast->global_scope = scope_new();
	ast->packages = arraylist_create();
	return ast;
}

void ast_free(AST *ast) {
	if (ast == NULL) return;
	scope_free(ast->global_scope);
	for (size_t i = 0; i < ast->packages->size; i++)
		package_free(arraylist_get(ast->packages, i));
	free(ast);
}

// AST *ast_load(CommandlineOptions *options) {
// 	AST *ast = ast_new();

// 	for (size_t i = 0; i < options->source_files->size; i++) {
// 		char *source_file_name = arraylist_get(options->source_files, i);

// 		if (!file_exists(source_file_name)) {
// 			log_error("source file '%s' does not exist\n", source_file_name);
// 			// free allocated memory
// 			for (size_t j = 0; j < ast->packages->size; j++) {
// 				Package *package = arraylist_get(ast->packages, j);
// 				package_free(package);
// 			}
// 			ast_free(ast);
// 			options_free(options);
// 			return -1;
// 		}

// 		char *source_code = read_file(source_file_name);
		
// 		if (source_code == NULL) {
// 			printf("    " RED "error: " RESET "cannot read file '%s'\n", source_file_name);
// 			continue;
// 		}

// 		// log_debug("parsing source file %s\n", arraylist_get(options->source_files, i));

// 		ArrayList *tokens = tokenize(source_code, source_file_name);
// 		if (tokens == NULL) {
// 			// free allocated memory
// 			// `source_file_name` does not need to be freed because it comes from the commandline
// 			free(source_code);
// 			ast_free(ast);
// 			return -1;
// 		}
		
// 		Package *package = parse(tokens, source_file_name);
// 		if (package == NULL) {
// 			// free allocated memory
// 			// `source_file_name` does not need to be freed because it comes from the commandline
// 			free(source_code);
// 			ast_free(ast);
// 			return -1;
// 		}

// 		arraylist_add(ast->packages, package);

// 		// log_debug("parsing complete for source file %s\n", source_file_name);

// 		free(source_code);
// 		for (size_t i = 0; i < tokens->size; i++)
// 			token_free(arraylist_get(tokens, i));
// 		arraylist_free(tokens);
// 	}

// 	return ast;
// }
