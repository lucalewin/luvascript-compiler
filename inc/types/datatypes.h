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

Datatype *parse_datatype(char *type);
void datatype_free(Datatype *datatype);

#endif