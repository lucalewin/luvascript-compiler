#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/expression.h>

typedef struct _Variable Variable;
typedef struct _VariableTemplate VariableTemplate;

struct _Variable {
	char *identifier;
	char *type_identifier;
	Expression_T *initializer;

	unsigned int is_constant : 1;
	unsigned int is_array : 1;
	unsigned int is_pointer : 1;

	// is only used when is_array is true
	unsigned long long int array_size;
};

// this should also be [Deprecated]
struct _VariableTemplate {
	char *identifier;
	char *type_identifier;

	unsigned int is_constant : 1;
	
	DatatypeOLD *datatype;
};

Variable *variable_new();

VariableTemplate *convert_to_variable_template(Variable *variable);
VariableTemplate *copy_variable_template(VariableTemplate *template_variable);

void variable_free(Variable *variable);
void variable_template_free(VariableTemplate *variable_template);

#endif // LUVA_TYPE_VARIABLE_H