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

void print_expr(Expr *expr) {
    if (expr == NULL) {
        printf("ERROR: Expression is NULL!\n");
        return;
    }
    
    switch (expr->type) {
        case EXPR_BASE:
            printf("expr{");
            print_expr(expr->e);
            printf("}");
            break;

        case EXPR_LITERAL: {
            LiteralExpr *l = expr->l;
            switch (l->type) {
                case IDENTIFIER:
                    printf("IDENTIFIER=%s", l->identifier);
                    break;
                case STRING:
                    printf("STRING=\"%s\"", l->string);
                    break;
                case NUMBER:
                    printf("NUMBER=%d", l->number);
                    break;
                default:
                    printf("ERROR: Unknown literal expression type: %d\n", l->type);
                    break;
            }
            break;
        }
        case EXPR_BINARY: {
            printf("binary-expr{left=");
            BinaryExpr *b = expr->b;
            print_expr(b->left);
            printf(",operator=%s,right=", b->op);
            print_expr(b->right);
            printf("}");
            break;
        }
        case EXPR_UNARY: {
            UnaryExpr *u = expr->u;
            printf("unary-expr{operator=%s,expression=", u->op);
            print_expr(u->expr);
            printf("}");
            break;
        }
        case EXPR_TERNARY:
            printf("WARNING #1 (expression.c)");
            break;
        default:
            printf("ERROR: Unknown expression type: %d\n", expr->type);
            break;
    }
}

// ----------------------------------------------------------------

LiteralExpr *literal_expr_create(literal_expr_t type, char *data) {
    LiteralExpr *expr = malloc(sizeof(LiteralExpr));
    expr->type = type;
    switch (type)
    {
        case IDENTIFIER:
            expr->identifier = malloc(strlen(data));
            strcpy(expr->identifier, data);
            break;
        case STRING:
            expr->string = malloc(strlen(data));
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