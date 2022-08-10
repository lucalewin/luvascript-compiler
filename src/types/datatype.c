#include <types/datatype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/util.h>

DataType *data_type_new(char *type_identifier, unsigned int size, enum _data_types type,
                        bool is_array, bool is_pointer, unsigned long long int array_size) {
    DataType *datatype = malloc(sizeof(DataType));
    datatype->identifier = strdup(type_identifier);
    datatype->size = size;
    datatype->type = type;
    datatype->is_array = is_array;
    datatype->is_pointer = is_pointer;
    datatype->array_size = array_size;
    return datatype;
}

void data_type_free(DataType *datatype) {
    if (datatype == NULL) {
        return;
    }

    free(datatype->identifier);
    free(datatype);
}

bool data_type_is_compatible(const DataType *a, const DataType *b) {
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
