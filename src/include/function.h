#ifndef LUVA_FUNCTION_H
#define LUVA_FUNCTION_H

// ------------------------------------

#include "arraylist.h"
#include "expression.h"
#include "statement.h"
#include "type.h"

// ------------------------------------

typedef struct _Func Function;
typedef struct _FuncParam FuncParam; 
typedef struct _FuncReturnType FuncReturnType;

// ------------------------------------

struct _Func {
    char *name;
    ArrayList *params;
    ArrayList *return_types;
    ArrayList *statements;
};

struct _FuncParam {
    Type type;
    char *var_name;
    Expr *default_value;
};

struct _FuncReturnType {
    Type type;
    Expr *default_value;
};

#endif // LUVA_FUNCTION_H