#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <types/statement.h>

typedef struct _function {
	char *identifier;
	ArrayList *parameters;
	int return_type;
	Scope *global_scope;
	Scope *local_scope;
	ArrayList *body_statements;
} Function;

#endif // LUVA_TYPES_FUNCTION_H