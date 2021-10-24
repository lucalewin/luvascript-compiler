#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <types/statement.h>
#include <types/datatypes.h>

typedef struct _function {
	char *identifier;
	ArrayList *parameters;
	int return_type_old;
	Datatype *return_type;
	Scope *global_scope;
	Scope *local_scope;
	ArrayList *body_statements;
	unsigned int is_extern : 1;
} Function;

typedef struct _func_template {
	char *identifier;
	ArrayList *param_datatypes;
	Datatype *return_type;
} FunctionTemplate;

#endif // LUVA_TYPES_FUNCTION_H