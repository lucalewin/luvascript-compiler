#ifndef LUVA_AST_H
#define LUVA_AST_H

#include "expression.h"

typedef struct _ast AST;

struct _ast {
    Expr *root;
};

#endif // LUVA_AST_H