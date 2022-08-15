#ifndef __LUVASCRIPT_TYPES_DATATYPE_H__
#define __LUVASCRIPT_TYPES_DATATYPE_H__

#include <stdbool.h>

typedef struct _data_type DataType;

/**
 * @brief 
 * 
 */
enum _data_types {
    DATA_TYPE_VOID,
    DATA_TYPE_INT,
    DATA_TYPE_UNSIGNED_INT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_BOOL,
    DATA_TYPE_CHAR,

    DATA_TYPE_STRUCT,
    DATA_TYPE_ENUM,

    DATA_TYPE_NONE,
    DATA_TYPE_UNKNOWN,
};

/**
 * @brief 
 * 
 */
struct _data_type {
    char *identifier;
    long long int size;
    enum _data_types type;

    bool is_array : 1;
    bool is_pointer : 1;
    
    long long int array_size;
    long long int pointer_depth;
};

/**
 * @brief TODO: add description
 * 
 * @param identifier 
 * @param size 
 * @param type 
 * @param is_array 
 * @param is_pointer 
 * @param array_size 
 * @param pointer_depth
 * @return DataType* 
 */
DataType *data_type_init(char *identifier, long long int size, enum _data_types type,
                        bool is_array, bool is_pointer, long long int array_size,
                        long long int pointer_depth);

/**
 * @brief TODO: add description
 * 
 * @param datatype 
 */
void data_type_free(DataType *datatype);

/**
 * @brief TODO: add description
 * 
 * @param identifier 
 * @return DataType* 
 */
DataType *data_type_parse(char *identifier);

/**
 * @brief TODO: add description
 * 
 * @param datatype 
 * @return DataType* 
 */
DataType *data_type_copy(DataType *datatype);

/**
 * @brief TODO: add description
 * 
 * @param identifier 
 * @return bool 
 */
bool data_type_is_primitive(char *identifier);

/**
 * @brief TODO: add description
 * 
 * @param a 
 * @param b 
 * @return bool
 */
bool data_type_is_compatible(const DataType *a, const DataType *b);

extern DataType PRIMITIVE_DATA_TYPES[];
extern const unsigned int PRIMITIVE_DATA_TYPES_COUNT;

#endif // __LUVASCRIPT_TYPES_DATATYPE_H__