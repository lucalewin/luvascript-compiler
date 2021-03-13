#ifndef LUVA_STATEMENT_H
#define LUVA_STATEMENT_H

// ---------------------------------

#include "expression.h"

// ---------------------------------

typedef enum _StatementType StatementType;
typedef struct _Statement Statement;
typedef struct _ExprStatement ExprStatement;
typedef struct _ReturnStatement ReturnStatement;

// ---------------------------------

enum _StatementType {
    STATEMENT_BLOCK,
    STATEMENT_EXPR,
    STATEMENT_RETURN
};

struct _Statement {
    StatementType type;
    void *statement;
};

struct _ExprStatement {
    Expr *expr;
};

struct _ReturnStatement {
    Expr *expr;
};

#endif // LUVA_STATEMENT_H