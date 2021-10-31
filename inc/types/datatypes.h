#ifndef LUVA_TYPES_DATATYPES_H
#define LUVA_TYPES_DATATYPES_H

#include <string.h>
#include <logging/logger.h>

typedef struct _datatype {
	char *type_identifier;
	unsigned int size;
	unsigned int is_primitive : 1;
} Datatype;

Datatype *parse_datatype(char *type);
void datatype_free(Datatype *datatype);

#endif