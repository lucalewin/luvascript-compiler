#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <types/statement.h>
#include <types/datatypes.h>

typedef struct _function {
	char *identifier;
	ArrayList *parameters;
	Datatype *return_type;
	ArrayList *body_statements;
	Scope *scope;
	unsigned int is_extern : 1;
} Function;

typedef struct _func_template {
	char *identifier;
	ArrayList *param_datatypes;
	Datatype *return_type;
} FunctionTemplate;

void function_free(Function *function);
void function_template_free(FunctionTemplate *template);

#endif // LUVA_TYPES_FUNCTION_H