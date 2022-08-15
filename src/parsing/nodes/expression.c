#include <parsing/nodes/expression.h>

#include <stdio.h>
#include <stdlib.h>

Expression *expression_new(ExpressionType type) {
    Expression *expression = malloc(sizeof(Expression));
    expression->type = type;
    expression->expr.literal = NULL;
    expression->expr.unary = NULL;
    expression->expr.binary = NULL;
    expression->expr.nested = NULL;
    expression->expr.function_call = NULL;
    expression->expr.assignment = NULL;
    expression->expr.array_access = NULL;
    expression->expr.member_access = NULL;
    expression->expr.list = NULL;
    return expression;
}

void expression_free(Expression *expression) {
	if (expression == NULL) {
		return;
	}

	switch (expression->type) {
		case EXPRESSION_TYPE_LITERAL:
			literal_free(expression->expr.literal);
			break;
		case EXPRESSION_TYPE_UNARY:
			literal_free(expression->expr.unary->identifier);
			break;
		case EXPRESSION_TYPE_BINARY:
			expression_free(expression->expr.binary->left);
			expression_free(expression->expr.binary->right);
			break;
		case EXPRESSION_TYPE_NESTED:
			expression_free(expression->expr.nested);
			break;
		case EXPRESSION_TYPE_FUNCTION_CALL:
			free(expression->expr.function_call->function_identifier);
			for (size_t i = 0; i < expression->expr.function_call->argument_expression_list->expressions->size; i++) {
				Expression *argument = arraylist_get(expression->expr.function_call->argument_expression_list->expressions, i);
				expression_free(argument);
			}
			break;
		case EXPRESSION_TYPE_ASSIGNMENT:
			expression_free(expression->expr.assignment->identifier);
			expression_free(expression->expr.assignment->assignment_value);
			break;
		case EXPRESSION_TYPE_ARRAY_ACCESS:
			literal_free(expression->expr.array_access->identifier);
			expression_free(expression->expr.array_access->index_expression);
			break;
		case EXPRESSION_TYPE_MEMBER_ACCESS:
			expression_free(expression->expr.member_access->identifier);
			free(expression->expr.member_access->member_identifier);
			break;
		case EXPRESSION_TYPE_LIST:
			for (size_t i = 0; i < expression->expr.list->expressions->size; i++) {
				Expression *expr = arraylist_get(expression->expr.list->expressions, i);
				expression_free(expr);
			}
			arraylist_free(expression->expr.list->expressions);
			free(expression->expr.list);
			break;
	}

	free(expression);
}

DataType *expression_get_datatype(Expression *expression) {
    if (expression == NULL) {
        return NULL;
    }
    switch (expression->type) {
        case EXPRESSION_TYPE_LITERAL:
            return expression->expr.literal->datatype;
        case EXPRESSION_TYPE_UNARY:
            return expression->expr.unary->datatype;
        case EXPRESSION_TYPE_BINARY:
            return expression->expr.binary->datatype;
        case EXPRESSION_TYPE_NESTED:
            return expression_get_datatype(expression->expr.nested);
        case EXPRESSION_TYPE_FUNCTION_CALL:
            return expression->expr.function_call->datatype;
        case EXPRESSION_TYPE_ASSIGNMENT:
            return expression->expr.assignment->datatype;
        case EXPRESSION_TYPE_ARRAY_ACCESS:
            return expression->expr.array_access->datatype;
        case EXPRESSION_TYPE_MEMBER_ACCESS:
            return expression->expr.member_access->datatype;
        case EXPRESSION_TYPE_LIST:
            return expression->expr.list->datatype;
    }
    return NULL;
}
