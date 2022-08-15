#include <parsing/nodes/function.h>

#include <parsing/scope_impl.h>
#include <parsing/nodes/datatypes.h>

Function *function_new() {
	Function *function = malloc(sizeof(Function));
	function->identifier = NULL;
	function->return_type = NULL;
	function->id = 0;
	function->parameters = arraylist_create();
	function->statements = arraylist_create();
	function->scope = NULL;
	function->is_extern = 0;
	function->is_imported = 0;
	function->filename = NULL;
	function->line = 0;
	function->column = 0;
	return function;
}

void function_free(Function *function) {
	if (function == NULL) return;

	free(function->identifier);
	data_type_free(function->return_type);

	for (size_t i = 0; i < arraylist_size(function->parameters); i++) {
		Variable *variable = arraylist_get(function->parameters, i);
		variable_free(variable);
	}

	for (size_t i = 0; i < arraylist_size(function->statements); i++) {
		Statement *statement = arraylist_get(function->statements, i);
		statement_free(statement);
	}

	arraylist_free(function->parameters);
	arraylist_free(function->statements);

	free(function);
}
