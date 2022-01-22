#include <types/ast.h>

#include <stdio.h>

#include <scope_impl.h>
#include <util/arraylist.h>
#include <types/package.h>

AST *ast_new() {
	AST *ast = calloc(1, sizeof(AST));
	ast->global_scope = NULL;
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
