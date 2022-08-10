#include <parsing/nodes/function.h>

#include <parsing/scope_impl.h>
#include <util/util.h>
#include <parsing/nodes/datatypes.h>

FunctionTemplate *convert_to_function_template(Function *func) {
	FunctionTemplate *func_template = calloc(1, sizeof(FunctionTemplate));
	func_template->identifier = strdup(func->identifier);
	func_template->return_type = strdup(func->return_type);
	func_template->id = func->id;
	// func_template->return_typeOLD = copy_datatype(func->return_typeOLD);
	func_template->parameter_types = arraylist_create();

	// convert function parameters to variable templates
	for (size_t j = 0; j < func->parameters->size; j++) {
		Variable *param = arraylist_get(func->parameters, j);
		// VariableTemplate *var_template = convert_to_variable_template(param);
		arraylist_add(func_template->parameter_types, strdup(param->type_identifier));
	}
	return func_template;
}

FunctionTemplate *copy_function_template(FunctionTemplate *template_function) {
	FunctionTemplate *new_func_template = calloc(1, sizeof(FunctionTemplate));
	
	new_func_template->identifier = strdup(template_function->identifier);
	new_func_template->return_type = strdup(template_function->return_type);
	new_func_template->id = template_function->id;
	// new_func_template->return_typeOLD = copy_datatype(template_function->return_typeOLD);
	new_func_template->parameter_types = arraylist_create();

	// copy function parameters
	for (size_t j = 0; j < template_function->parameter_types->size; j++) {
		DatatypeOLD *param_datatype = arraylist_get(template_function->parameter_types, j);
		arraylist_add(new_func_template->parameter_types, copy_datatype(param_datatype));
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
	size_t param_count = function_template->parameter_types->size;
	size_t param_ident_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		DatatypeOLD *datatype = arraylist_get(function_template->parameter_types, i);
		param_ident_length += strlen(datatype->type_identifier);
	}
	size_t extra_length_for_array_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		DatatypeOLD *datatype = arraylist_get(function_template->parameter_types, i);
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
		DatatypeOLD *datatype = arraylist_get(function_template->parameter_types, i);
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
	free(function->return_type);

	if (function->parameters != NULL) {
		for (size_t i = 0; i < function->parameters->size; i++) {
			Variable *parameter = arraylist_get(function->parameters, i);
			variable_free(parameter);
		}
		arraylist_free(function->parameters);
	}
	if (function->statements != NULL) {
		for (size_t i = 0; i < function->statements->size; i++) {
			Statement *statement = arraylist_get(function->statements, i);
			statement_free(statement);
		}
		arraylist_free(function->statements);
	}

	// datatype_free(function->return_typeOLD);
	scope_free(function->scope);

	free(function);
}

void function_template_free(FunctionTemplate *function_template) {
	free(function_template->identifier);

	for (size_t i = 0; i < function_template->parameter_types->size; i++) {
		DatatypeOLD *datatype = arraylist_get(function_template->parameter_types, i);
		datatype_free(datatype);
	}
}