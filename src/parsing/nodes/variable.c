#include <parsing/nodes/variable.h>

#include <stdbool.h>

#include <util/util.h>
#include <parsing/nodes/datatypes.h>


Variable *variable_new() {
	Variable *variable = malloc(sizeof(Variable));
	variable->identifier = NULL;
	variable->type = NULL;
	variable->initializer = NULL;
	variable->is_constant = false;
	variable->filename = NULL;
	variable->line = 0;
	variable->column = 0;
	return variable;
}

void variable_free(Variable *variable) {
	if (variable == NULL) {
		return;
	}

	free(variable->identifier);
	data_type_free(variable->type);
	expression_free(variable->initializer);
	free(variable);
}
