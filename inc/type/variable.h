#ifndef LUVA_VARIABLE_H
#define LUVA_VARIABLE_H

#include <expression.h>

typedef struct _VariableDecl VariableDecl; 

struct _VariableDecl {
    int is_constant;
    char *identifier;
    Expression_T *value;
};

#endif