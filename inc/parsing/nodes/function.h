#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/datatypes.h>

typedef struct _function {
	char *identifier;
	unsigned long long int id;
	ArrayList *parameters;
	Datatype *return_type;
	ArrayList *body_statements;
	Scope *scope;
	unsigned int is_extern : 1;
	unsigned int is_imported : 1;
} Function;

typedef struct _func_template {
	char *identifier;
	unsigned long long int id;
	ArrayList *param_datatypes;
	Datatype *return_type;
} FunctionTemplate;

FunctionTemplate *convert_to_function_template(Function *function);
FunctionTemplate *copy_function_template(FunctionTemplate *template_function);

char *function_to_lcc_identifier(FunctionTemplate *function_template);

void function_free(Function *function);
void function_template_free(FunctionTemplate *template);

#endif // LUVA_TYPES_FUNCTION_H