#include <types/datatypes.h>

#include <stdio.h>
#include <stdlib.h>

Datatype *parse_datatype(char *type) {
	Datatype *dt = calloc(1, sizeof(Datatype));

	if (strcmp(type, "byte") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "byte";
		dt->size = 1; // 1 byte
	} else if (strcmp(type, "short") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "short";
		dt->size = 2; // 2 bytes
	} else if (strcmp(type, "int") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "int";
		dt->size = 4; // 4 bytes
	} else if (strcmp(type, "long") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "long";
		dt->size = 8; // 8 bytes
	} else if (strcmp(type, "bool") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "bool";
		dt->size = 1; // 1 byte
	} else if (strcmp(type, "char") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "char";
		dt->size = 1; // 1 byte (ascii) (might change in future versions)
	} else if (strcmp(type, "string") == 0) {
		// a string is an array of chars
		dt->is_primitive = 1;
		dt->type_identifier = "string";
		dt->size = 1; // 1 byte (size of a char (in ascii)) (might change in future versions)
	} else if (strcmp(type, "void") == 0) {
		dt->is_primitive = 1;
		dt->type_identifier = "void";
		dt->size = 0; // 0 bytes
	} else {
		// log_error("unknown datatype: '%s'\n", type);
		free(dt);
		return NULL;
	}

	return dt;
}
