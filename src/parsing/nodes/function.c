#include <parsing/nodes/function.h>

#include <parsing/scope_impl.h>
#include <util/util.h>
#include <parsing/nodes/datatypes.h>

FunctionTemplate *convert_to_function_template(Function *func) {
	FunctionTemplate *func_template = calloc(1, sizeof(FunctionTemplate));
	func_template->identifier = strdup(func->identifier);
	func_template->id = func->id;
	func_template->return_type = copy_datatype(func->return_type);
	func_template->param_datatypes = arraylist_create();

	// convert function parameters to variable templates
	for (size_t j = 0; j < func->parameters->size; j++) {
		Variable *param = arraylist_get(func->parameters, j);
		// VariableTemplate *var_template = convert_to_variable_template(param);
		arraylist_add(func_template->param_datatypes, copy_datatype(param->type));
	}
	return func_template;
}

FunctionTemplate *copy_function_template(FunctionTemplate *template_function) {
	FunctionTemplate *new_func_template = calloc(1, sizeof(FunctionTemplate));
	
	new_func_template->identifier = strdup(template_function->identifier);
	new_func_template->id = template_function->id;
	new_func_template->return_type = copy_datatype(template_function->return_type);
	new_func_template->param_datatypes = arraylist_create();

	// copy function parameters
	for (size_t j = 0; j < template_function->param_datatypes->size; j++) {
		Datatype *param_datatype = arraylist_get(template_function->param_datatypes, j);
		arraylist_add(new_func_template->param_datatypes, copy_datatype(param_datatype));
	}
	return new_func_template;
}

/**
 * @brief implementation according to the `Luvascript Naming Convention` (LNC)
 * [https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/luvascript-naming-convention/]
 * 
 * @param function_template the function_template to convert to a lcc identifier
 * @return char* the lcc identifier
 */
char *function_to_lcc_identifier(FunctionTemplate *function_template) {
	size_t func_ident_length = strlen(function_template->identifier);
	size_t param_count = function_template->param_datatypes->size;
	size_t param_ident_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(function_template->param_datatypes, i);
		param_ident_length += strlen(datatype->type_identifier);
	}
	size_t extra_length_for_array_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(function_template->param_datatypes, i);
		if (datatype->is_array) {
			extra_length_for_array_length += 2;
		}
	}
	// 2 + param_count is the amount of underscores in the identifier
	size_t identifier_length = func_ident_length + param_ident_length + 2 + param_count + 5 + extra_length_for_array_length;
	char *identifier = calloc(identifier_length + 1, sizeof(char));
	strcpy(identifier, "_func_");
	strcat(identifier, function_template->identifier);
	strcat(identifier, "_");
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(function_template->param_datatypes, i);
		strcat(identifier, "_");
		strcat(identifier, datatype->type_identifier);

		if (datatype->is_array) {
			strcat(identifier, "@1");
		}
	}
	return identifier;
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