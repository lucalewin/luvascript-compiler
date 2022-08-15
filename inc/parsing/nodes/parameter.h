#ifndef __LUVASCRIPT_NODES_PARAMETER_H__
#define __LUVASCRIPT_NODES_PARAMETER_H__

#include <types/datatype.h>

typedef struct _parameter {
    char* name;
    DataType type;

    // node info
    char *filename;
    int line;
    int column;
} Parameter;

Parameter *parameter_new();
void parameter_free(Parameter *parameter);

#endif // __LUVASCRIPT_NODES_PARAMETER_H__