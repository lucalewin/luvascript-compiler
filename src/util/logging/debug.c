#include <util/logging/debug.h>

#include <stdio.h>

#include <util/util.h>
#include <parsing/nodes/literal.h>
#include <util/logging/logger.h>

void print_literal(Literal *literal) {
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

void print_expression(Expression *expression) {
    printf("\"type\":\"%s\",\"value\":", EXPRESSION_TYPES[expression->type]);
    switch (expression->type) {
        case EXPRESSION_TYPE_LITERAL:
            print_literal(expression->expr.literal);
            break;
        case EXPRESSION_TYPE_UNARY: {
			printf("{\"operator\":%d,\"value\":\"%s\"}", expression->expr.unary->operator, expression->expr.unary->identifier->value);
            break;
        }
        case EXPRESSION_TYPE_BINARY: {
            printf("{\"left\":{");
            print_expression(expression->expr.binary->left);
            printf("},\"operator\":%d,\"right\":{", expression->expr.binary->operator);
            print_expression(expression->expr.binary->right);
            printf("}}");
            break;
        }
        case EXPRESSION_TYPE_NESTED: {
            printf("{");
            print_expression(expression->expr.nested);
            printf("}");
            break;
        }
        case EXPRESSION_TYPE_FUNCTION_CALL: {
            printf("{\"name\":\"%s\",\"arguments\":[", expression->expr.function_call->function_identifier);
            for (int i = 0; i < arraylist_size(expression->expr.function_call->argument_expression_list->expressions); i++) {
                printf("{");
                print_expression(arraylist_get(expression->expr.function_call->argument_expression_list->expressions, i));
                printf("}");
                if (i < arraylist_size(expression->expr.function_call->argument_expression_list->expressions) - 1) {
                    printf(",");
                }
            }
            printf("]}");
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
			CompoundStatement *compound_stmt = statement->stmt.compound;

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
            print_expression(statement->stmt.expression->expression);
			printf("}");
            break;
        case STATEMENT_RETURN:
			printf("\"expression\":{");
            print_expression(statement->stmt._return->expression);
			printf("}");
            break;
		case STATEMENT_VARIABLE_DECLARATION: {
			Variable *var = statement->stmt.variable_declaration->variable;
			printf("\"identifier\":\"%s\",\"type\":%s,\"initializer\":{", var->identifier, var->type->identifier);
			print_expression(var->initializer);
			printf("}");
			break;
		}
        case STATEMENT_CONDITIONAL: {
            ConditionalStatement *conditional_stmt = statement->stmt.conditional;
            printf("\"condition\":{");
            print_expression(conditional_stmt->condition);
            printf("},\"true_branch\":{");
            print_statement(conditional_stmt->true_branch);
            if (conditional_stmt->false_branch != NULL) {
                printf("},\"false_branch\":{");
                print_statement(conditional_stmt->false_branch);
            }
            printf("}");
            break;
        }
        default:
            log_error("unknown statement type: %d", statement->type);
            break;
    }
}

void print_variable(Variable *var) {
    printf("{\"identifier\":\"%s\",\"type\":%s,\"initializer\":{", var->identifier, var->type->identifier);
    print_expression(var->initializer);
    printf("}}");
}