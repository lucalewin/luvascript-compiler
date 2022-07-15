#ifndef LUVA_TYPES_DATATYPES_H
#define LUVA_TYPES_DATATYPES_H

#include <string.h>
#include <logging/logger.h>

typedef struct _datatype {
	char *type_identifier;
	unsigned int size;
	unsigned int is_primitive : 1;
	unsigned int is_array : 1;
	unsigned int is_pointer : 1;
	unsigned long long int array_size;
} Datatype;

typedef enum _DatatypeDirective {
	DATATYPE_DIRECTIVE_BYTE = 1,
	DATATYPE_DIRECTIVE_WORD = 2,
	DATATYPE_DIRECTIVE_DWORD = 4,
	DATATYPE_DIRECTIVE_QWORD = 8
} DatatypeDirective;

int types_equal(const Datatype *a, const Datatype *b);
int datatype_is_number(const Datatype *datatype);

Datatype *parse_datatype(char *type);
Datatype *copy_datatype(const Datatype *datatype);

void datatype_free(Datatype *datatype);

char *to_datatype_directive(const Datatype *datatype);
char *datatype_to_datatype_directive(const Datatype *datatype);

#endif