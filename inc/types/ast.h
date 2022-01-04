#ifndef LUVA_AST_H
#define LUVA_AST_H

#include <scope.h>
#include <util/arraylist.h>

typedef struct _ast AST;

struct _ast {
	Scope *global_scope;
	ArrayList *functions;
	ArrayList *global_variables;
	ArrayList *extern_functions;

	ArrayList *packages;
};

void ast_free(AST *ast);

#endif // LUVA_AST_H