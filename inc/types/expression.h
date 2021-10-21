#ifndef LUVA_EXPRESSION_H
#define LUVA_EXPRESSION_H

#include <types/operator.h>
#include <types/literal.h>
#include <util/arraylist.h>

typedef enum ExpressionType ExpressionType;
typedef struct Expression Expression_T;
typedef struct UnaryExpression UnaryExpression_T;
typedef struct BinaryExpression BinaryExpression_T;
typedef struct NestedExpression NestedExpression_T;
typedef struct FunctionCallExpression FunctionCallExpression_T;

enum ExpressionType {
    EXPRESSION_LITERAL,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY,
    EXPRESSION_NESTED,
	EXPRESSION_FUNCTION_CALL
};

struct Expression {
    ExpressionType type;

    union {
        Literal_T *literal_expr;
        UnaryExpression_T *unary_expr;
        BinaryExpression_T *binary_expr;
        NestedExpression_T *nested_expr;
		FunctionCallExpression_T *func_call_expr;
    } expr;
};

struct UnaryExpression {
    Literal_T *identifier;
    UnaryOperator_T operator;
};

struct BinaryExpression {
    Expression_T *expression_left;
    BinaryOperator operator;
    Expression_T *expression_right;
};

struct NestedExpression {
    Expression_T *expression;
};

struct FunctionCallExpression {
	char *function_identifier;
	ArrayList *argument_expression_list;
};

extern const char *EXPRESSION_TYPES[];

#endif // LUVA_EXPRESSION_H