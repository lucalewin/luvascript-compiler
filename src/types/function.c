#include <types/function.h>

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