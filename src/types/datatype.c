#include <types/datatype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/util.h>

DataType DATATYPE_I32 = { "i32", 4, DATA_TYPE_INT, false, false, 0, 0 };
DataType *DATATYPE_I32_PTR = &DATATYPE_I32;

DataType PRIMITIVE_DATA_TYPES[] = {
    { "void", 0, DATA_TYPE_VOID, false, false, 0, 0 },
    { "bool", 1, DATA_TYPE_BOOL, false, false, 0, 0 },

    { "i8",  1, DATA_TYPE_INT, false, false, 0, 0 },
    { "i16", 2, DATA_TYPE_INT, false, false, 0, 0 },
    { "i32", 4, DATA_TYPE_INT, false, false, 0, 0 },
    { "i64", 8, DATA_TYPE_INT, false, false, 0, 0 },

    { "u8",  1, DATA_TYPE_UNSIGNED_INT, false, false, 0, 0 },
    { "u16", 2, DATA_TYPE_UNSIGNED_INT, false, false, 0, 0 },
    { "u32", 4, DATA_TYPE_UNSIGNED_INT, false, false, 0, 0 },
    { "u64", 8, DATA_TYPE_UNSIGNED_INT, false, false, 0, 0 },

    { "f32", 4, DATA_TYPE_FLOAT, false, false, 0, 0 },
    { "f64", 8, DATA_TYPE_FLOAT, false, false, 0, 0 },

    { "char", 1, DATA_TYPE_CHAR, false, false, 0, 0 },
    { "string", 1, DATA_TYPE_CHAR, true, true, 1, 0 },
};

const unsigned int PRIMITIVE_DATA_TYPES_COUNT = sizeof(PRIMITIVE_DATA_TYPES) / sizeof(DataType);

DataType *data_type_init(char *type_identifier, long long int size, enum _data_types type,
                        bool is_array, bool is_pointer, long long int array_size,
                        long long int pointer_depth) {
    DataType *datatype = malloc(sizeof(DataType));
    datatype->identifier = strdup(type_identifier);
    datatype->size = size;
    datatype->type = type;
    datatype->is_array = is_array;
    datatype->is_pointer = is_pointer;
    datatype->array_size = array_size;
    datatype->pointer_depth = pointer_depth;
    return datatype;
}

void data_type_free(DataType *datatype) {
    if (datatype == NULL) {
        return;
    }

    free(datatype->identifier);
    free(datatype);
}

DataType *data_type_parse(char *identifier) {

    if (data_type_is_primitive(identifier)) {
        DataType *primitive = NULL;

        for (size_t i = 0; i < PRIMITIVE_DATA_TYPES_COUNT; i++) {
            if (strcmp(PRIMITIVE_DATA_TYPES[i].identifier, identifier) == 0) {
                primitive = &PRIMITIVE_DATA_TYPES[i];
                break;
            }
        }

        if (primitive == NULL) return NULL;

        return data_type_init(identifier, primitive->size, primitive->type, primitive->is_array,
                primitive->is_pointer, primitive->array_size, primitive->pointer_depth);
    }

    return data_type_init(identifier, -1, DATA_TYPE_UNKNOWN, false, false, 0, 0);
}

DataType *data_type_copy(DataType *datatype) {
    if (datatype == NULL) {
        return NULL;
    }

    return data_type_init(datatype->identifier, datatype->size, datatype->type,
            datatype->is_array, datatype->is_pointer, datatype->array_size,
            datatype->pointer_depth);
}

bool data_type_is_primitive(char *identifier) {
    if (identifier == NULL) {
        return false;
    }

    // check if the list of primitive types contains the identifier
    for (size_t i = 0; i < PRIMITIVE_DATA_TYPES_COUNT; i++) {
        if (strcmp(PRIMITIVE_DATA_TYPES[i].identifier, identifier) == 0) {
            return true;
        }
    }

    return false;
}

bool data_type_is_compatible(const DataType *a, const DataType *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    // if the super types are not equal, then the types are not compatible
    if (a->type != b->type) {
        return false;
    }

    // validate that the type identifiers are not NULL
    if (a->identifier == NULL || b->identifier == NULL) {
        return false;
    }

    // check if the type identifiers are equal
    if (strcmp(a->identifier, b->identifier) == 0) {
        return true;
    }

    // the super types are equal but the identifiers are different
    // this can be the case when the super type is DATA_TYPE_INT, 
    // DATA_TYPE_UNSIGNED_INT or DATA_TYPE_FLOAT
    // but they are compatible by the compiler definition
    // They are not compatible if the super type is DATA_TYPE_STRUCT
    // or DATA_TYPE_ENUM
    if (a->type == DATA_TYPE_STRUCT || a->type == DATA_TYPE_ENUM) {
        return false;
    }

    return true;
}
