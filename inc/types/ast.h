#ifndef LUVA_AST_H
#define LUVA_AST_H

#include <scope.h>
#include <types/statement.h>

typedef struct _ast AST;

struct _ast {
	Scope *global_scope;
	Statement *statement;
};

#endif // LUVA_AST_H