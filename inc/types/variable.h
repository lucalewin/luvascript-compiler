#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <types/datatypes.h>
#include <types/literal.h>
#include <types/expression.h>

typedef struct _Variable Variable;

struct _Variable {
	Datatype *datatype;
	Literal_T *identifier;
	Expression_T *default_value;
	unsigned int is_constant : 1;
};

#endif // LUVA_TYPE_VARIABLE_H