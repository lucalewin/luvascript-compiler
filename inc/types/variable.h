#ifndef LUVA_TYPE_VARIABLE_H
#define LUVA_TYPE_VARIABLE_H

#include <types/literal.h>
#include <types/expression.h>

typedef struct _Variable Variable;

struct _Variable {
	int datatype;
	Literal_T *identifier;
	Expression_T *default_value;
};

#endif // LUVA_TYPE_VARIABLE_H