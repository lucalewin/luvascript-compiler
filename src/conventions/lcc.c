#include <conventions/lcc.h>

#include <stdlib.h>
#include <string.h>

char *variable_as_lcc_identifier(Variable *var) {
    size_t var_ident_length = strlen(var->identifier);
	size_t datatype_ident_length = strlen(var->type->identifier);
	size_t identifier_length = var_ident_length + datatype_ident_length + 7;
	size_t array_length = var->type->is_array ? 2 : 0;
	char *identifier = calloc(identifier_length + 1 + array_length, sizeof(char));
	strcpy(identifier, "_var_");
	strcat(identifier, var->identifier);
	strcat(identifier, "__");
	strcat(identifier, var->type->identifier);
	if (var->type->is_array) {
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
		
        param_ident_length += strlen(parameter->type->identifier);

        if (parameter->type->is_array) {
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

		strcat(identifier, "_");
		strcat(identifier, parameter->type->identifier);

		if (parameter->type->is_array) {
			strcat(identifier, "@1");
		}
	}
	return identifier;
}

// char *functiontemplate_as_lcc_identifier(FunctionTemplate *func) {
// 	size_t func_ident_length = strlen(func->identifier);
// 	size_t param_count = func->parameter_types->size;
// 	size_t param_ident_length = 0;
// 	for (size_t i = 0; i < param_count; i++) {
// 		// DatatypeOLD *datatype = arraylist_get(func->parameter_types, i);
// 		// param_ident_length += strlen(datatype->type_identifier);
// 		DataType *datatype = arraylist_get(func->parameter_types, i);
// 		param_ident_length += strlen(datatype->identifier);
// 	}
// 	size_t extra_length_for_array_length = 0;
// 	for (size_t i = 0; i < param_count; i++) {
// 		// DatatypeOLD *datatype = arraylist_get(func->parameter_types, i);
// 		DataType *datatype = arraylist_get(func->parameter_types, i);
// 		if (datatype->is_array) {
// 			extra_length_for_array_length += 2;
// 		}
// 	}
// 	// 2 + param_count is the amount of underscores in the identifier
// 	size_t identifier_length = func_ident_length + param_ident_length + 2 + param_count + 5 + extra_length_for_array_length;
// 	char *identifier = calloc(identifier_length + 1, sizeof(char));
// 	strcpy(identifier, "_func_");
// 	strcat(identifier, func->identifier);
// 	strcat(identifier, "_");
// 	for (size_t i = 0; i < param_count; i++) {
// 		// DatatypeOLD *datatype = arraylist_get(func->parameter_types, i);
// 		DataType *datatype = arraylist_get(func->parameter_types, i);
// 		strcat(identifier, "_");
// 		strcat(identifier, datatype->identifier);

// 		if (datatype->is_array) {
// 			strcat(identifier, "@1");
// 		}
// 	}
// 	return identifier;
// }

// -------------------------------------------------------------------------

char *enum_definition_as_lcc_identifier(Enum *enum_definition) {
	const char *format = "_enum_%s";
	size_t length = snprintf(NULL, 0, format, enum_definition->name) + 1;
	char *identifier = calloc(length, sizeof(char));
	sprintf(identifier, format, enum_definition->name);
	return identifier;
}

char *enum_definition_member_as_lcc_identifier(EnumDefinitionMember *enum_definition_member) {
	const char *format = "._enum_member_%s";
	size_t length = snprintf(NULL, 0, format, enum_definition_member->name) + 1;
	char *identifier = calloc(length, sizeof(char));
	sprintf(identifier, format, enum_definition_member->name);
	return identifier;
}
