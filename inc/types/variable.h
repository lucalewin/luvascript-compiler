#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <types/datatypes.h>
#include <types/literal.h>
#include <types/expression.h>

typedef struct _Variable Variable;
typedef struct _VariableTemplate VariableTemplate;

struct _Variable {
	Datatype *datatype;
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

#endif // LUVA_TYPE_VARIABLE_H