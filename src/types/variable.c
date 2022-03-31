#include <types/variable.h>

/**
 * @brief TODO (lucalewin): add documentation for function
 * 
 * @param variable 
 * @return VariableTemplate* 
 */
VariableTemplate *convert_to_variable_template(Variable *variable) {
	VariableTemplate *template = calloc(1, sizeof(VariableTemplate));

	template->identifier = variable->identifier->value;
	template->datatype = variable->datatype;
	template->is_constant = variable->is_constant;

	return template;
}

void variable_free(Variable *variable) {
	if (variable == NULL) {
		return;
	}
	datatype_free(variable->datatype);
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