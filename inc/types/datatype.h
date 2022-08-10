#ifndef __LUVASCRIPT_TYPES_DATATYPE_H__
#define __LUVASCRIPT_TYPES_DATATYPE_H__

#include <stdbool.h>

typedef struct _data_type DataType;

enum _data_types {
    DATA_TYPE_NONE,
    DATA_TYPE_VOID,
    DATA_TYPE_INT,
    DATA_TYPE_UNSIGNED_INT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_BOOL,
    DATA_TYPE_CHAR,

    DATA_TYPE_STRUCT,
    DATA_TYPE_ENUM,
};

struct _data_type {
    char *identifier;
    unsigned int size;
    enum _data_types type;

    bool is_array : 1;
    bool is_pointer : 1;
    
    unsigned long long int array_size;
};

DataType *data_type_new(char *identifier, unsigned int size, enum _data_types type,
                        bool is_array, bool is_pointer, unsigned long long int array_size);
void data_type_free(DataType *datatype);

bool data_type_is_compatible(const DataType *a, const DataType *b);

#endif // __LUVASCRIPT_TYPES_DATATYPE_H__