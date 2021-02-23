#include "include/expression.h"

Expr *literal_expr_to_expr(LiteralExpr *literal_expr) {
    Expr *expr = malloc(sizeof(Expr));
    expr->type = EXPR_LITERAL;
    expr->l = literal_expr;
    return expr;
}

// ----------------------------------------------------------------

Expr *binary_expr_to_expr(BinaryExpr *binary_expr) {
    Expr *expr = malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->b = binary_expr;
    return expr;
}

// ----------------------------------------------------------------

LiteralExpr *literal_expr_create(literal_expr_t type, char *data) {
    LiteralExpr *expr = malloc(sizeof(LiteralExpr));
    expr->type = type;
    switch (type)
    {
        case IDENTIFIER:
            strcpy(expr->identifier, data);
            break;
        case STRING:
            strcpy(expr->string, data);
            break;
        case NUMBER:
            expr->number = get_int(data);
            break;
        default:
            break;
    }
    return expr;
}

// ----------------------------------------------------------------

BinaryExpr *binary_expr_create(Expr *left, char *op, Expr *right) {
    BinaryExpr *expr = malloc(sizeof(BinaryExpr));
    expr->left = left;
    expr->op = op;
    expr->right = right;
    return expr;
}

// ----------------------------------------------------------------

UnaryExpr *unary_expr_create(char *op, Expr *expr) {
    UnaryExpr *u_expr = malloc(sizeof(BinaryExpr));
    u_expr->op = op;
    u_expr->expr;
    return u_expr;
}

// TernaryExpr *ternary_expr_create(Expr *expr_cond, Expr *expr_true, Expr *expr_false) {
// TODO
// }