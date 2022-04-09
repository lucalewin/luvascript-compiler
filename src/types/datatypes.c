#include <types/datatypes.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/arraylist.h>
#include <util/util.h>

int types_equal(const Datatype *a, const Datatype *b) {
	if (a == NULL || b == NULL) return 0;
	if (a->type_identifier == NULL || b->type_identifier == NULL) {
		return 0;
	}
	return strcmp(a->type_identifier, b->type_identifier) == 0;
}

int datatype_is_number(const Datatype *datatype) {
	if (datatype == NULL) return 0;
	if (strcmp(datatype->type_identifier, "byte") == 0 || 
				strcmp(datatype->type_identifier, "short") == 0 || 
				strcmp(datatype->type_identifier, "int") == 0 || 
				strcmp(datatype->type_identifier, "long") == 0) {
		// TODO: check for unsigned numbers
		return 1;
	}
	return 0;
}

Datatype *parse_datatype(char *type) {
	Datatype *dt = calloc(1, sizeof(Datatype));

	if (strcmp(type, "byte") == 0 || strcmp(type, "i8") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "byte";
		dt->is_pointer = 0;
		dt->size = 1; // 1 byte
	} else if (strcmp(type, "short") == 0 || strcmp(type, "i16") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "short";
		dt->is_pointer = 0;
		dt->size = 2; // 2 bytes
	} else if (strcmp(type, "int") == 0 || strcmp(type, "i32") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "int";
		dt->is_pointer = 0;
		dt->size = 4; // 4 bytes
	} else if (strcmp(type, "long") == 0 || strcmp(type, "i64") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "long";
		dt->is_pointer = 0;
		dt->size = 8; // 8 bytes
	} else if (strcmp(type, "bool") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "bool";
		dt->is_pointer = 0;
		dt->size = 1; // 1 byte
	} else if (strcmp(type, "char") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "char";
		dt->is_pointer = 0;
		dt->size = 1; // 1 byte (ascii) (might change in future versions)
	} else if (strcmp(type, "string") == 0) {
		// a string is an array of chars
		dt->is_primitive = 1;
		dt->type_identifier = "string";
		dt->is_pointer = 1; // pointer to char array
		dt->size = 1; // 1 byte for ascii character (might change in future versions)
	} else if (strcmp(type, "void") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "void";
		dt->is_pointer = 0;
		dt->size = 0; // 0 bytes
	} else {
		log_error("unknown datatype: '%s'\n", type);
		free(dt);
		return NULL;
	}

	return dt;
}

Datatype *copy_datatype(const Datatype *datatype) {
	Datatype *dt = calloc(1, sizeof(Datatype));

	dt->is_primitive = datatype->is_primitive;
	dt->is_array = datatype->is_array;
	dt->is_pointer = datatype->is_pointer;
	dt->size = datatype->size;
	dt->array_size = datatype->array_size;
	dt->type_identifier = strdup(datatype->type_identifier);

	return dt;
}

void datatype_free(Datatype *datatype) {
	if (datatype == NULL) return;
	if (!datatype->is_primitive)
		free(datatype->type_identifier);
	free(datatype);
}
