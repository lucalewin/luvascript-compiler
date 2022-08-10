#include <types/table.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DataType PRIMITIVE_DATA_TYPES[] = {
    { "void", 0, DATA_TYPE_VOID, false, false, 0 },
    { "bool", 1, DATA_TYPE_BOOL, false, false, 0 },

    { "i8",  1, DATA_TYPE_INT, false, false, 0 },
    { "i16", 2, DATA_TYPE_INT, false, false, 0 },
    { "i32", 4, DATA_TYPE_INT, false, false, 0 },
    { "i64", 8, DATA_TYPE_INT, false, false, 0 },

    { "u8",  1, DATA_TYPE_UNSIGNED_INT, false, false, 0 },
    { "u16", 2, DATA_TYPE_UNSIGNED_INT, false, false, 0 },
    { "u32", 4, DATA_TYPE_UNSIGNED_INT, false, false, 0 },
    { "u64", 8, DATA_TYPE_UNSIGNED_INT, false, false, 0 },

    { "f32", 4, DATA_TYPE_FLOAT, false, false, 0 },
    { "f64", 8, DATA_TYPE_FLOAT, false, false, 0 },

    { "char", 1, DATA_TYPE_CHAR, false, false, 0 },
    { "string", 1, DATA_TYPE_CHAR, true, false, 0 },
};

const unsigned int PRIMITIVE_DATA_TYPES_COUNT = sizeof(PRIMITIVE_DATA_TYPES) / sizeof(DataType);

struct _data_type_table {
    ArrayList *data_types;
};

DataTypeTable *data_type_table_new() {
    DataTypeTable *table = malloc(sizeof(DataTypeTable));
    table->data_types = arraylist_create();
    return table;
}

void data_type_table_free(DataTypeTable *table) {
    if (table == NULL) {
        return;
    }
    
    if (table->data_types != NULL) {
        for (size_t i = 0; i < arraylist_size(table->data_types); i++) {
            DataType *datatype = arraylist_get(table->data_types, i);
            data_type_free(datatype);
        }
        arraylist_free(table->data_types);
    }
    free(table);
}

bool data_type_table_add(DataTypeTable *table, char *type_identifier, unsigned int size, enum _data_types type) {
    if (table == NULL) {
        return false;
    }
    
    if (data_type_table_contains(table, type_identifier)) {
        return false;
    }
    
    DataType *datatype = data_type_new(type_identifier, size, type, false, false, 0);
    arraylist_add(table->data_types, datatype);
    return true;
}

bool data_type_table_contains(DataTypeTable *table, char *type_identifier) {
    if (table == NULL) {
        return false;
    }
    
    if (data_type_table_isPrimitive(table, type_identifier)) {
        return true;
    }

    for (size_t i = 0; i < arraylist_size(table->data_types); i++) {
        DataType *datatype = arraylist_get(table->data_types, i);
        if (strcmp(datatype->identifier, type_identifier) == 0) {
            return true;
        }
    }
    return false;
}

bool data_type_table_isPrimitive(DataTypeTable *table, char *type_type_identifier) {
    for (size_t i = 0; i < PRIMITIVE_DATA_TYPES_COUNT; i++) {
        if (strcmp(PRIMITIVE_DATA_TYPES[i].identifier, type_type_identifier) == 0) {
            return true;
        }
    }
    return false;
}

DataType *data_type_table_get(DataTypeTable *table, char *type_identifier) {
    // first check the primitive types
    for (size_t i = 0; i < PRIMITIVE_DATA_TYPES_COUNT; i++) {
        if (strcmp(PRIMITIVE_DATA_TYPES[i].identifier, type_identifier) == 0) {
            return &PRIMITIVE_DATA_TYPES[i];
        }
    }

    // then check the user defined types
    for (size_t i = 0; i < arraylist_size(table->data_types); i++) {
        DataType *datatype = arraylist_get(table->data_types, i);
        if (strcmp(datatype->identifier, type_identifier) == 0) {
            return datatype;
        }
    }

    return NULL;
}
