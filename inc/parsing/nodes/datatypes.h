#ifndef LUVA_TYPES_DATATYPES_H
#define LUVA_TYPES_DATATYPES_H

#include <string.h>
#include <util/logging/logger.h>

typedef enum _datatype_types {
	DATATYPE_TYPE_VOID,
	DATATYPE_TYPE_BOOL,
	DATATYPE_TYPE_INT,
	DATATYPE_TYPE_UINT,
	DATATYPE_TYPE_FLOAT,
	DATATYPE_TYPE_CHAR,
	DATATYPE_TYPE_ENUM,
	DATATYPE_TYPE_ENUM_MEMBER,
	DATATYPE_TYPE_STRUCT,
	DATATYPE_TYPE_UNKNOWN,
} DatatypeTypes;

typedef struct _datatype {
	char *type_identifier;
	unsigned int size;
	DatatypeTypes type;
	unsigned int is_primitive : 1;
	unsigned int is_array : 1;
	unsigned int is_pointer : 1;
	unsigned long long int array_size;
} DatatypeOLD;

int types_equal(const DatatypeOLD *a, const DatatypeOLD *b);
int datatype_is_number(const DatatypeOLD *datatype);

DatatypeOLD *parse_datatype(char *type);
DatatypeOLD *copy_datatype(const DatatypeOLD *datatype);

void datatype_free(DatatypeOLD *datatype);

char *to_datatype_directive(const DatatypeOLD *datatype);
char *datatype_to_datatype_directive(const DatatypeOLD *datatype);

#endif