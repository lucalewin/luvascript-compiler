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
