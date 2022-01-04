#include <logging/debug.h>

#include <stdio.h>

#include <util/util.h>
#include <types/literal.h>
#include <logging/logger.h>

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
        case EXPRESSION_TYPE_LITERAL:
            print_literal(expression->expr.literal_expr);
            break;
        case EXPRESSION_TYPE_UNARY: {
			printf("{\"operator\":%d,\"value\":\"%s\"}", expression->expr.unary_expr->operator, expression->expr.unary_expr->identifier->value);
            break;
        }
        case EXPRESSION_TYPE_BINARY: {
            printf("{\"left\":{");
            print_expression(expression->expr.binary_expr->expression_left);
            printf("},\"operator\":%d,\"right\":{", expression->expr.binary_expr->operator);
            print_expression(expression->expr.binary_expr->expression_right);
            printf("}}");
            break;
        }
        case EXPRESSION_TYPE_NESTED: {
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
    printf("\"type\":\"%s\",", STATEMENT_TYPES[statement->type]);
    switch (statement->type) {
        case STATEMENT_COMPOUND: {
			CompoundStatement *compound_stmt = statement->stmt.compound_statement;

			printf("\"statements\":[");
			for (int i = 0; i < compound_stmt->nested_statements->size; i++) {
				printf("{");
				Statement *stmt = arraylist_get(compound_stmt->nested_statements, i);
				print_statement(stmt);
				printf("}");
				if (i + 1 < compound_stmt->nested_statements->size) {
					printf(",");
				}
			}
			printf("]");
            break;
		}
        case STATEMENT_EXPRESSION:
			printf("\"expression\":{");
            print_expression(statement->stmt.expression_statement->expression);
			printf("}");
            break;
        case STATEMENT_RETURN:
			printf("\"expression\":{");
            print_expression(statement->stmt.return_statement->expression);
			printf("}");
            break;
		case STATEMENT_VARIABLE_DECLARATION: {
			Variable *var = statement->stmt.variable_decl->variable;
			printf("\"identifier\":\"%s\",\"type\":%s,\"default_value\":{", var->identifier->value, var->datatype->type_identifier);
			print_expression(var->default_value);
			printf("}");
			break;
		}
        default:
            log_error("unknown statement type: %d", statement->type);
            break;
    }
}
