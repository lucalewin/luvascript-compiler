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