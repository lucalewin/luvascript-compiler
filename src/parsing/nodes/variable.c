#include <parsing/nodes/variable.h>

#include <util/util.h>
#include <parsing/nodes/datatypes.h>

/**
 * @brief TODO (lucalewin): add documentation for function
 * 
 * @param variable 
 * @return VariableTemplate* 
 */
VariableTemplate *convert_to_variable_template(Variable *variable) {
	VariableTemplate *template = calloc(1, sizeof(VariableTemplate));

	template->identifier = strdup(variable->identifier->value);
	template->datatype = copy_datatype(variable->type);
	template->is_constant = variable->is_constant;

	return template;
}

VariableTemplate *copy_variable_template(VariableTemplate *template_variable) {
	VariableTemplate *new_template = calloc(1, sizeof(VariableTemplate));

	new_template->identifier = strdup(template_variable->identifier);
	new_template->datatype = copy_datatype(template_variable->datatype);
	new_template->is_constant = template_variable->is_constant;

	return new_template;
}

/**
 * @brief implementation according to the `Luvascript Naming Convention` (LNC)
 * [https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/luvascript-naming-convention/]
 * 
 * @param var_template the VariableTemplate to convert to a lcc identifier
 * @return char* the lcc identifier
 */
char *variable_to_lcc_identifier(VariableTemplate *var_template) {
	size_t var_ident_length = strlen(var_template->identifier);
	size_t datatype_ident_length = strlen(var_template->datatype->type_identifier);
	size_t identifier_length = var_ident_length + datatype_ident_length + 7;
	size_t array_length = var_template->datatype->is_array ? 2 : 0;
	char *identifier = calloc(identifier_length + 1 + array_length, sizeof(char));
	strcpy(identifier, "_var_");
	strcat(identifier, var_template->identifier);
	strcat(identifier, "__");
	strcat(identifier, var_template->datatype->type_identifier);
	if (var_template->datatype->is_array) {
		strcat(identifier, "@1");
	}
	return identifier;
}

void variable_free(Variable *variable) {
	if (variable == NULL) {
		return;
	}
	datatype_free(variable->type);
	literal_free(variable->identifier);
	expression_free(variable->default_value);
	free(variable);
}

void variable_template_free(VariableTemplate *variable_template) {
	if (variable_template == NULL) {
		return;
	}
	free(variable_template->identifier);
	free(variable_template);
}