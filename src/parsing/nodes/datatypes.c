#include <parsing/nodes/datatypes.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/arraylist.h>
#include <util/util.h>

// int types_equal(const DatatypeOLD *a, const DatatypeOLD *b) {
// 	if (a == NULL || b == NULL) return 0;
// 	if (a->type_identifier == NULL || b->type_identifier == NULL) {
// 		return 0;
// 	}
// 	return strcmp(a->type_identifier, b->type_identifier) == 0;
// }

// int datatype_is_number(const DatatypeOLD *datatype) {
// 	if (datatype == NULL) return 0;
// 	if (strcmp(datatype->type_identifier, "i8") == 0 || 
// 				strcmp(datatype->type_identifier, "i16") == 0 || 
// 				strcmp(datatype->type_identifier, "i32") == 0 || 
// 				strcmp(datatype->type_identifier, "i64") == 0) {
// 		// TODO: check for unsigned numbers
// 		return 1;
// 	}
// 	return 0;
// }

// DatatypeOLD *parse_datatype(char *type) {
// 	DatatypeOLD *dt = calloc(1, sizeof(DatatypeOLD));

// 	if (strcmp(type, "i8") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "i8";
// 		dt->type = DATATYPE_TYPE_INT;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 1; // 1 byte
// 	} else if (strcmp(type, "i16") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "i16";
// 		dt->type = DATATYPE_TYPE_INT;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 2; // 2 bytes
// 	} else if (strcmp(type, "i32") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "i32";
// 		dt->type = DATATYPE_TYPE_INT;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 4; // 4 bytes
// 	} else if (strcmp(type, "i64") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "i64";
// 		dt->type = DATATYPE_TYPE_INT;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 8; // 8 bytes
// 	} else if (strcmp(type, "bool") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "bool";
// 		dt->type = DATATYPE_TYPE_BOOL;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 1; // 1 byte
// 	} else if (strcmp(type, "char") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "char";
// 		dt->type = DATATYPE_TYPE_CHAR;
// 		dt->is_pointer = 0;
// 		dt->is_array = 0;
// 		dt->size = 1; // 1 byte (ascii) (might change in future versions)
// 	} else if (strcmp(type, "string") == 0) {
// 		// a string is an array of chars
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "char";
// 		dt->type = DATATYPE_TYPE_CHAR;
// 		dt->is_pointer = 0;
// 		dt->is_array = 1; // array of chars
// 		dt->size = 1; // 1 byte for ascii character (might change in future versions)
// 	} else if (strcmp(type, "void") == 0) {
// 		dt->is_primitive = 1;
// 		dt->type_identifier = "void";
// 		dt->type = DATATYPE_TYPE_VOID;
// 		dt->is_pointer = 0;
// 		dt->size = 0; // 0 bytes
// 	} else {
// 		// log_error("unknown datatype: '%s'\n", type);
// 		// free(dt);
// 		// return NULL;
// 		dt->is_primitive = 0;
// 		dt->type_identifier = strdup(type);
// 		dt->type = DATATYPE_TYPE_UNKNOWN;
// 		dt->is_pointer = 0;
// 		dt->size = -1; // unknown (will be calculated later)
// 		dt->is_array = 0;
// 	}

// 	return dt;
// }

// DatatypeOLD *copy_datatype(const DatatypeOLD *datatype) {
// 	DatatypeOLD *dt = calloc(1, sizeof(DatatypeOLD));

// 	dt->type_identifier = strdup(datatype->type_identifier);
// 	dt->size = datatype->size;
// 	dt->type = datatype->type;
// 	dt->is_primitive = datatype->is_primitive;
// 	dt->is_array = datatype->is_array;
// 	dt->is_pointer = datatype->is_pointer;
// 	dt->array_size = datatype->array_size;

// 	return dt;
// }

// void datatype_free(DatatypeOLD *datatype) {
// 	if (datatype == NULL) return;
// 	if (!datatype->is_primitive)
// 		free(datatype->type_identifier);
// 	free(datatype);
// }
