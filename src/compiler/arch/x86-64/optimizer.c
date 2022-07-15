#include <x86-64/optimizer.h>

#include <stdio.h>
#include <stdlib.h>

#include <types/expression.h>
#include <util/util.h>
#include <logging/logger.h>

Expression_T *simplify_expression(Expression_T *expr) {
	switch (expr->type) {
		case EXPRESSION_TYPE_LITERAL:
			return expr;

		case EXPRESSION_TYPE_BINARY: {

			BinaryExpression_T *bin_expr = expr->expr.binary_expr;
			Expression_T *left = bin_expr->expression_left;
			Expression_T *right = bin_expr->expression_right;

			if (bin_expr->expression_left->type != EXPRESSION_TYPE_LITERAL) {
				// try to simplify expression
				left = simplify_expression(bin_expr->expression_left);
			}
			if (bin_expr->expression_right->type != EXPRESSION_TYPE_LITERAL) {
				// try to simplify expression
				right = simplify_expression(bin_expr->expression_right);
			}

			if (left == NULL || right == NULL) {
				log_error("simplify_expression(): an unknown error occured\n");
				exit(1);
			}

			if (left->type != EXPRESSION_TYPE_LITERAL || right->type != EXPRESSION_TYPE_LITERAL) {
				// log_debug("simplify_expression(): could not simplify expression: \n\ttype 1: %d\n\ttype 2: %d\n", left->type, right->type);
				return expr;
			}

			if (left->expr.literal_expr->type != LITERAL_NUMBER || right->expr.literal_expr->type != LITERAL_NUMBER) {
				return expr;
			}

			// both literals are numbers -> evaluate the expression

			char *end;

			signed long long int num_left = strtoll(left->expr.literal_expr->value, &end, 10); //get_int(left->expr.literal_expr->value);
			signed long long int num_right = strtoll(right->expr.literal_expr->value, &end, 10); //get_int(right->expr.literal_expr->value);
			signed long long int result;

			switch(bin_expr->operator) {
				case BINARY_OPERATOR_ADD:
					result = num_left + num_right;
					break;
				case BINARY_OPERATOR_SUBTRACT:
					result = num_left - num_right;
					break;
				case BINARY_OPERATOR_MULTIPLY:
					result = num_left * num_right;
					break;
				case BINARY_OPERATOR_DIVIDE:
					result = num_left / num_right;
					break;
				case BINARY_OPERATOR_MODULO:
					return expr;
				case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
				case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
					// currently not supported, because some c compilers use arithmetic shift and some use logical shift
					return expr;
				case BINARY_OPERATOR_BITWISE_AND:
					result = num_left & num_right;
					break;
				case BINARY_OPERATOR_BITWISE_XOR:
					result = num_left ^ num_right;
					break;
				case BINARY_OPERATOR_BITWISE_OR:
					result = num_left | num_right;
					break;
				case BINARY_OPERATOR_LOGICAL_EQUAL:
				case BINARY_OPERATOR_LOGICAL_NOT_EQUAL:
				case BINARY_OPERATOR_LOGICAL_LESS:
				case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL:
				case BINARY_OPERATOR_LOGICAL_GREATER:
				case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
					// currently not supported
					return expr;
				default:
					log_error("unexpected binary expression operator '%d'\n", bin_expr->operator);
					return NULL;
			}

			size_t length = snprintf(NULL, 0, "%lld", result);
			char *result_as_string = malloc(length + 1);
			if (result_as_string == NULL) {
				log_error("unable to allocate memory for string\n");
				exit(1);
			}
			snprintf(result_as_string, length + 1, "%lld", result);

			Literal_T *literal = calloc(1, sizeof(Literal_T));
			if (literal == NULL) {
				log_error("simplify_expression(): unable to allocate memory for literal\n");
				free(result_as_string);
				exit(1);
			}
			literal->type = LITERAL_NUMBER;
			literal->value = result_as_string;

			expr->type = EXPRESSION_TYPE_LITERAL;
			expr->expr.literal_expr = literal;

			return expr;
		}

		case EXPRESSION_TYPE_NESTED: {
			// log_warning("simplify_expression(): simplification for nested expression is not implemented yet\n");
			return simplify_expression(expr->expr.nested_expr->expression);
		}

		case EXPRESSION_TYPE_UNARY: {
			// log_warning("simplify_expression(): simplification for unary expressions is not implemented yet\n");
			return expr;
		}

		case EXPRESSION_TYPE_FUNCTIONCALL: {
			// log_warning("simplify_expression(): simplification for function call expressions is not implemented yet\n");
			return expr;
		}

		case EXPRESSION_TYPE_ASSIGNMENT:
			return expr;

		case EXPRESSION_TYPE_ARRAYACCESS:
			return expr;

		case EXPRESSION_TYPE_MEMBERACCESS:
			return expr;

		case EXPRESSION_TYPE_LIST:
			return expr;
	}
	
	return expr;
}
