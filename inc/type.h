#pragma once
#ifndef LUVA_TYPE_H
#define LUVA_TYPE_H

typedef enum _Type Type;
typedef struct _DataType DataType;

enum _Type {
    TYPE_PRIMITIVE,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_CLASS
};

struct _DataType {
    Type type;
    char *type_name;
};

#endif // LUVA_TYPE_H