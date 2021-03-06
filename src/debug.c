#include "include/debug.h"

void debugPrintTab(int amount) {
    for (int i = 0; i < amount; i++) {
        printf("    ");
    }
}

void debugPrintExpression(Expr *expr, int depth) {
    if (expr == NULL) {
        printf("ERROR #221: Expression is NULL!\n");
        return;
    }
    
    switch (expr->type) {
        case EXPR_BASE:
            printf("expr{");
            debugPrintExpression(expr->e, depth+1);
            printf("}");
            break;

        case EXPR_LITERAL: {
            LiteralExpr *l = expr->l;
            // debugPrintTab(depth);
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
                    printf("ERROR #222: Unknown literal expression type: %d\n", l->type);
                    break;
            }
            break;
        }
        case EXPR_BINARY: {
            printf("binary-expr{\n");
            debugPrintTab(depth);
            printf("left=");
            debugPrintExpression(expr->b->left, depth+1);
            printf(",\n");
            debugPrintTab(depth);
            printf("operator=%s,\n", expr->b->op);
            debugPrintTab(depth);
            printf("right=");
            debugPrintExpression(expr->b->right, depth+1);
            printf("\n");
            debugPrintTab(depth-1);
            printf("}");
            break;
        }
        case EXPR_UNARY: {
            UnaryExpr *u = expr->u;
            printf("unary-expr{\n");
            debugPrintTab(depth);
            printf("operator=%s,\n", expr->u->op);
            debugPrintTab(depth);
            printf("expression=");
            debugPrintExpression(expr->u->expr, depth+1);
            printf("\n");
            debugPrintTab(depth-1);
            printf("}");
            break;
        }
        case EXPR_TERNARY:
            printf("WARNING #1 (expression.c)");
            break;
        default:
            printf("ERROR #223: Unknown expression type: %d\n", expr->type);
            break;
    }
    if (depth == 1) {
        printf("\n");
    }
}

// void debugPrintStatement(Statement *st);
// void debugPrintFunction(Function *func);