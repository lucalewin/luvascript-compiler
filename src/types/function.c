#include <types/function.h>

#include <scope_impl.h>

FunctionTemplate *convert_to_function_template(Function *func) {
	FunctionTemplate *func_template = calloc(1, sizeof(FunctionTemplate));
	func_template->identifier = func->identifier;
	func_template->return_type = func->return_type;
	func_template->param_datatypes = arraylist_create();

	// convert function parameters to variable templates
	for (size_t j = 0; j < func->parameters->size; j++) {
		Variable *param = arraylist_get(func->parameters, j);
		VariableTemplate *var_template = convert_to_variable_template(param);
		arraylist_add(func_template->param_datatypes, var_template);
	}
	return func_template;
}

void function_free(Function *function) {
	if (function == NULL) return;

	free(function->identifier);

	if (function->parameters != NULL) {
		for (size_t i = 0; i < function->parameters->size; i++) {
			Variable *parameter = arraylist_get(function->parameters, i);
			variable_free(parameter);
		}
		arraylist_free(function->parameters);
	}
	if (function->body_statements != NULL) {
		for (size_t i = 0; i < function->body_statements->size; i++) {
			Statement *statement = arraylist_get(function->body_statements, i);
			statement_free(statement);
		}
		arraylist_free(function->body_statements);
	}

	datatype_free(function->return_type);
	scope_free(function->scope);

	free(function);
}

void function_template_free(FunctionTemplate *function_template) {
	free(function_template->identifier);

	for (size_t i = 0; i < function_template->param_datatypes->size; i++) {
		Datatype *datatype = arraylist_get(function_template->param_datatypes, i);
		datatype_free(datatype);
	}
}