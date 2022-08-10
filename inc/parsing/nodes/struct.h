#ifndef __LUVA_NODE_STRUCT_H__
#define __LUVA_NODE_STRUCT_H__

#include <util/arraylist.h>

typedef struct _struct_definition {
    char *name;
    ArrayList *members;
} StructDefinition;



#endif