#ifndef LUVA_EXPR_H
#define LUVA_EXPR_H

// -----------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// -----------------------------------------------

typedef enum _ExprType expr_t;
typedef enum _LiteralExprType literal_expr_t;

typedef struct _Expr Expr;
typedef struct _LiteralExpr LiteralExpr;
typedef struct _BinaryExpr BinaryExpr;
typedef struct _UnaryExpr UnaryExpr;
typedef struct _TernaryExpr TernaryExpr;

// -----------------------------------------------

enum _ExprType {
    EXPR_BASE,
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_TERNARY
};

enum _LiteralExprType {
    IDENTIFIER,
    STRING,
    NUMBER
};

// -----------------------------------------------

struct _Expr {
    expr_t type;
    union {
        Expr *e;
        LiteralExpr *l;
        BinaryExpr *b;
        UnaryExpr *u;
        TernaryExpr *t;
    };
};

Expr *literal_expr_to_expr(LiteralExpr *literal_expr);
Expr *binary_expr_to_expr(BinaryExpr *binary_expr);

void print_expr(Expr *expr);

// -----------------------------------------------

struct _LiteralExpr {
    literal_expr_t type;
    union {
        char *identifier;
        char *string;
        int number;
    };
};

LiteralExpr *literal_expr_create(literal_expr_t type, char *data);

// -----------------------------------------------

struct _BinaryExpr {
    Expr *left;
    char *op;
    Expr *right;
};

BinaryExpr *binary_expr_create(Expr *left, char *op, Expr *right);

// -----------------------------------------------

struct _UnaryExpr {
    char *op;
    Expr *expr;
};

UnaryExpr *unary_expr_create(char *op, Expr *expr);

// -----------------------------------------------

struct _TernaryExpr {
    Expr *condition;
    Expr *expr_true;
    Expr *expr_false;
};

TernaryExpr *ternary_expr_create(Expr *expr_cond, Expr *expr_true, Expr *expr_false);

#endif // LUVA_EXPR_H