#include <parsing/nodes/variable.h>

#include <stdbool.h>

#include <util/util.h>
#include <parsing/nodes/datatypes.h>


Variable *variable_new() {
	Variable *variable = malloc(sizeof(Variable));
	variable->identifier = NULL;
	variable->type_identifier = NULL;
	variable->initializer = NULL;
	variable->is_constant = false;
	variable->is_array = false;
	variable->is_pointer = false;
	variable->array_size = 0;
	return variable;
}

VariableTemplate *convert_to_variable_template(Variable *variable) {
	VariableTemplate *template = calloc(1, sizeof(VariableTemplate));

	// template->identifier = strdup(variable->identifierOLD->value);
	// template->datatype = copy_datatype(variable->typeOLD);
	template->identifier = strdup(variable->identifier);
	template->is_constant = variable->is_constant;

	return template;
}

VariableTemplate *copy_variable_template(VariableTemplate *template_variable) {
	VariableTemplate *new_template = calloc(1, sizeof(VariableTemplate));

	new_template->identifier = strdup(template_variable->identifier);
	// new_template->datatype = copy_datatype(template_variable->datatype);
	new_template->is_constant = template_variable->is_constant;


	return new_template;
}

void variable_free(Variable *variable) {
	if (variable == NULL) {
		return;
	}
	// datatype_free(variable->typeOLD);
	// literal_free(variable->identifierOLD);
	free(variable->identifier);
	free(variable->type_identifier);
	expression_free(variable->initializer);
	free(variable);
}

void variable_template_free(VariableTemplate *variable_template) {
	if (variable_template == NULL) {
		return;
	}
	free(variable_template->identifier);
	free(variable_template);
}