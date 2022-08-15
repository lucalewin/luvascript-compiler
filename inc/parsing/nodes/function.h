#ifndef LUVA_TYPES_FUNCTION_H
#define LUVA_TYPES_FUNCTION_H

#include <util/arraylist.h>
#include <types/datatype.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/variable.h>

typedef struct _function {
	char *identifier;      // the funtion name
	DataType *return_type; // the return type of the function
	unsigned int id;       // the id of the function
	ArrayList *parameters; // ArrayList<Variable>
	ArrayList *statements; // ArrayList<Statement>
	Scope *scope;          // the scope of the function

	unsigned int is_extern : 1;   // 1, if function was declared as extern
	unsigned int is_imported : 1; // 1, if function was imported from another file

	// node info
	char *filename;
	int line;
	int column;
} Function;

Function *function_new();
void function_free(Function *function);

#endif // LUVA_TYPES_FUNCTION_H