#ifndef __LUVASCRIPT_TYPES_TABLE_H__
#define __LUVASCRIPT_TYPES_TABLE_H__

#include <stdbool.h>

#include <util/arraylist.h>
#include <types/datatype.h>

typedef struct _data_type_table DataTypeTable;

DataTypeTable *data_type_table_new();
void data_type_table_free(DataTypeTable *table);

bool data_type_table_add(DataTypeTable *table, char *type_identifier, unsigned int size, enum _data_types type);
bool data_type_table_contains(DataTypeTable *table, char *type_identifier);
bool data_type_table_isPrimitive(DataTypeTable *table, char *type_type_identifier);
DataType *data_type_table_get(DataTypeTable *table, char *type_identifier);

extern DataType PRIMITIVE_DATA_TYPES[];
extern const unsigned int PRIMITIVE_DATA_TYPES_COUNT;

#endif // __LUVASCRIPT_TYPES_TABLE_H__