#ifndef LUVA_FUNCTION_H
#define LUVA_FUNCTION_H

// ------------------------------------

#include "statement.h"
#include "type.h"

// ------------------------------------

typedef struct _Func Function;
typedef struct _FuncParam FuncParam; 
typedef struct _FuncReturnType FuncReturnType;

// ------------------------------------

struct Func {
    char *name;
    char *return_type;
    // FuncParam* params
    FuncReturnType *return_types;
    Statement *root_statement; 
};

struct _FuncParam {
    Type type;
    char *var_name;
    void *default_value;
};

struct _FuncReturnType {
    Type type;
    void *default_value;
};

#endif // LUVA_FUNCTION_H