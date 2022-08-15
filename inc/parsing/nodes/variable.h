#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <parsing/nodes/node.h>
#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/expression.h>

#include <types/datatype.h>

typedef struct _Variable Variable;

struct _Variable {
	char *identifier;             // the variable name
	DataType *type;               // the type of the variable
	Expression *initializer;      // the initializer of the variable
	unsigned int is_constant : 1; // 1, if function was declared as const

	// node info
	char *filename;
	int line;
	int column;
};

Variable *variable_new();
void variable_free(Variable *variable);

#endif // LUVA_TYPE_VARIABLE_H