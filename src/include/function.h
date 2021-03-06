#ifndef LUVA_FUNCTION_H
#define LUVA_FUNCTION_H

// ------------------------------------

#include "statement.h"

// ------------------------------------

typedef struct Func {
    char *name;
    char *return_type;
    // FuncParam* params
    Statement *root_statement; 
} Function;

#endif // LUVA_FUNCTION_H