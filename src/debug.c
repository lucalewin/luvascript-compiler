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
                    debugPrintTab(depth);
                    printf("\"type\": \"LiteralExpression\",\n");
                    debugPrintTab(depth);
                    printf("\"literal-type\": \"Identifier\",\n");
                    debugPrintTab(depth);
                    printf("\"value\": \"%s\"\n", l->identifier);
                    break;
                case STRING:
                    debugPrintTab(depth);
                    printf("\"type\": \"LiteralExpression\",\n");
                    debugPrintTab(depth);
                    printf("\"literal-type\": \"String\",\n");
                    debugPrintTab(depth);
                    printf("\"value\": \"%s\"\n", l->string);
                    break;
                case NUMBER:
                    debugPrintTab(depth);
                    printf("\"type\": \"LiteralExpression\",\n");
                    debugPrintTab(depth);
                    printf("\"literal-type\": \"Number\",\n");
                    debugPrintTab(depth);
                    printf("\"value\": %d\n", l->number);
                    break;
                default:
                    printf("ERROR #222: Unknown literal expression type: %d\n", l->type);
                    break;
            }
            break;
        }
        case EXPR_BINARY: {
            // debugPrintTab(depth-1);
            // printf("\"expression\": {\n");
            debugPrintTab(depth);
            printf("\"type\": \"BinaryExpression\"\n");
            debugPrintTab(depth);
            printf("\"expression\": {\n");
            debugPrintExpression(expr->b->left, depth+1);
            debugPrintTab(depth);
            printf("},\n");
            debugPrintTab(depth);
            printf("\"operator\": \"%s\",\n", expr->b->op);
            debugPrintTab(depth);
            printf("\"expression\": {\n");
            debugPrintExpression(expr->b->right, depth+1);
            debugPrintTab(depth);
            printf("}\n");
            // debugPrintTab(depth-1);
            // printf("}\n");
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

void debugPrintStatement(Statement *st, int depth) {
    debugPrintTab(depth-1);
    printf("\"statement\": {\n");
    debugPrintTab(depth);
    switch (st->type)
    {
    case STATEMENT_BLOCK:
    printf("HELLO\n");
        /* code */
        break;
    
    case STATEMENT_EXPR: {
        ExprStatement *e_stmt = st->statement;
        printf("\"type\": \"ExpressionStatement\",\n");
        debugPrintTab(depth);
        printf("\"expression\": {\n");
        debugPrintExpression(e_stmt->expr, depth + 1);
        debugPrintTab(depth);
        printf("}\n");
        break;
    }
    case STATEMENT_RETURN: {
        ReturnStatement *r_stmt = st->statement;
        printf("\"type\": \"ReturnStatement\",\n");
        debugPrintExpression(r_stmt->expr, depth + 1);
        break;
    }
    default:
        printf("Unknown type: %d\n", st->type);
        break;
    }
    debugPrintTab(depth - 1);
    printf("},\n");
}

void debugPrintFuncParam(FuncParam *func_param, int depth) {
    debugPrintTab(depth);
    printf("\"name\": \"%s\",\n", func_param->var_name);
    debugPrintTab(depth);
    printf("\"type\": \"%d\",\n", func_param->type);
    if (func_param->default_value != NULL) {
        debugPrintTab(depth);
        printf("\"default-value-expr\": {\n");
        debugPrintExpression(func_param->default_value, depth + 1);
        debugPrintTab(depth);
        printf("}\n");
    }
}

void debugPrintFunction(Function *func, int depth) {
    // name
    debugPrintTab(depth);
    printf("\"name\": \"%s\"\n", func->name);
    // params

    // return types

    // statements
}