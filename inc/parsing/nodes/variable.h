#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/literal.h>
#include <parsing/nodes/expression.h>

typedef struct _Variable Variable;
typedef struct _VariableTemplate VariableTemplate;

struct _Variable {
	Datatype *type;
	Literal_T *identifier;
	Expression_T *default_value;
	unsigned int is_constant : 1;
};

struct _VariableTemplate {
	Datatype *datatype;
	char *identifier;
	unsigned int is_constant : 1;
};

VariableTemplate *convert_to_variable_template(Variable *variable);
VariableTemplate *copy_variable_template(VariableTemplate *template_variable);

void variable_free(Variable *variable);
void variable_template_free(VariableTemplate *variable_template);

#endif // LUVA_TYPE_VARIABLE_H