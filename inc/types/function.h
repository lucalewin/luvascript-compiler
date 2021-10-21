#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <types/statement.h>

// TODO: add parameters to function

typedef struct _function {
	char *identifier;
	ArrayList *parameter;
	int return_type;
	Statement *body;
} Function;

#endif // LUVA_TYPES_FUNCTION_H