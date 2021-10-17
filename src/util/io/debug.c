#include <stdio.h>
#include <logger.h>
#include <debug.h>
#include <util.h>
#include <types/literal.h>

void print_literal(Literal_T *literal) {
    printf("{\"type\":\"%s\",\"value\":", LITERAL_TYPES[literal->type]);
    switch (literal->type) {
        case LITERAL_IDENTIFIER:
            printf("\"%s\"", literal->value);
            break;
        case LITERAL_NUMBER:
            printf("%s", literal->value);
            break;
        case LITERAL_STRING:
            printf("\"%s\"", literal->value);
            break;
        default:
            break;
    }
    printf("}");
}

void print_expression(Expression_T *expression) {
    printf("\"type\":\"%s\",\"value\":", EXPRESSION_TYPES[expression->type]);
    switch (expression->type) {
        case EXPRESSION_LITERAL:
            print_literal(expression->expr.literal_expr);
            break;
        case EXPRESSION_UNARY: {
			printf("{\"operator\":%d,\"value\":\"%s\"}", expression->expr.unary_expr->operator, expression->expr.unary_expr->identifier->value);
            break;
        }
        case EXPRESSION_BINARY: {
            printf("{\"left\":{");
            print_expression(expression->expr.binary_expr->expression_left);
            printf("},\"operator\":%d,\"right\":{", expression->expr.binary_expr->operator);
            print_expression(expression->expr.binary_expr->expression_right);
            printf("}}");
            break;
        }
        case EXPRESSION_NESTED: {
            // log_warning("TODO: printing of nested expression is not implemented yet\n");
            printf("{");
            print_expression(expression->expr.nested_expr->expression);
            printf("}");
            break;
        }
        default:
            break;
    }
}

void print_statement(Statement *statement) {
    printf("\"type\":\"%s\",\"expression\":{", STATEMENT_TYPES[statement->type]);
    switch (statement->type) {
        case STATEMENT_COMPOUND:
            // TODO
            log_warning("TODO: implement debug printing for compound statements\n");
            break;
        case STATEMENT_EXPRESSION:
            print_expression(statement->stmt.expression_statement->expression);
            break;
        case STATEMENT_RETURN:
            print_expression(statement->stmt.return_statement->return_expression);
            break;
        default:
            log_error("unknown statement type: %d", statement->type);
            break;
    }
    printf("}");
}
