#ifndef LUVA_STATEMENT_H
#define LUVA_STATEMENT_H

#include "expression.h"

typedef struct _Statement {
    Expr *expr;
} Statement;

#endif // LUVA_STATEMENT_H