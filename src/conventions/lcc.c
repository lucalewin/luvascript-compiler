#include <conventions/lcc.h>

#include <stdlib.h>
#include <string.h>

char *variable_as_lcc_identifier(Variable *var) {
    size_t var_ident_length = strlen(var->identifier->value);
	size_t datatype_ident_length = strlen(var->type->type_identifier);
	size_t identifier_length = var_ident_length + datatype_ident_length + 7;
	size_t array_length = var->type->is_array ? 2 : 0;
	char *identifier = calloc(identifier_length + 1 + array_length, sizeof(char));
	strcpy(identifier, "_var_");
	strcat(identifier, var->identifier->value);
	strcat(identifier, "__");
	strcat(identifier, var->type->type_identifier);
	if (var->type->is_array) {
		strcat(identifier, "@1");
	}
	return identifier;
}

char *variabletemplate_as_lcc_identifier(VariableTemplate *var) {
	size_t var_ident_length = strlen(var->identifier);
	size_t datatype_ident_length = strlen(var->datatype->type_identifier);
	size_t identifier_length = var_ident_length + datatype_ident_length + 7;
	size_t array_length = var->datatype->is_array ? 2 : 0;
	char *identifier = calloc(identifier_length + 1 + array_length, sizeof(char));
	strcpy(identifier, "_var_");
	strcat(identifier, var->identifier);
	strcat(identifier, "__");
	strcat(identifier, var->datatype->type_identifier);
	if (var->datatype->is_array) {
		strcat(identifier, "@1");
	}
	return identifier;
}

// -------------------------------------------------------------------------

char *function_as_lcc_identifier(Function *func) {
    size_t func_ident_length = strlen(func->identifier);
	size_t param_count = func->parameters->size;
	size_t param_ident_length = 0;

	for (size_t i = 0; i < param_count; i++) {
        Variable *parameter = arraylist_get(func->parameters, i);
		Datatype *datatype = parameter->type;
		
        param_ident_length += strlen(datatype->type_identifier);

        if (datatype->is_array) {
			param_ident_length += 2;
		}
	}

	// 2 + param_count is the amount of underscores in the identifier
	size_t identifier_length = func_ident_length
                             + param_ident_length + 2
                             + param_count + 5; // + extra_length_for_array_length;

	char *identifier = calloc(identifier_length + 1, sizeof(char));

	strcpy(identifier, "_func_");
	strcat(identifier, func->identifier);
	strcat(identifier, "_");

	for (size_t i = 0; i < param_count; i++) {
		Variable *parameter = arraylist_get(func->parameters, i);
		Datatype *datatype = parameter->type;

		strcat(identifier, "_");
		strcat(identifier, datatype->type_identifier);

		if (datatype->is_array) {
			strcat(identifier, "@1");
		}
	}
	return identifier;
}

char *functiontemplate_as_lcc_identifier(FunctionTemplate *func) {
	size_t func_ident_length = strlen(func->identifier);
	size_t param_count = func->param_datatypes->size;
	size_t param_ident_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(func->param_datatypes, i);
		param_ident_length += strlen(datatype->type_identifier);
	}
	size_t extra_length_for_array_length = 0;
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(func->param_datatypes, i);
		if (datatype->is_array) {
			extra_length_for_array_length += 2;
		}
	}
	// 2 + param_count is the amount of underscores in the identifier
	size_t identifier_length = func_ident_length + param_ident_length + 2 + param_count + 5 + extra_length_for_array_length;
	char *identifier = calloc(identifier_length + 1, sizeof(char));
	strcpy(identifier, "_func_");
	strcat(identifier, func->identifier);
	strcat(identifier, "_");
	for (size_t i = 0; i < param_count; i++) {
		Datatype *datatype = arraylist_get(func->param_datatypes, i);
		strcat(identifier, "_");
		strcat(identifier, datatype->type_identifier);

		if (datatype->is_array) {
			strcat(identifier, "@1");
		}
	}
	return identifier;
}
