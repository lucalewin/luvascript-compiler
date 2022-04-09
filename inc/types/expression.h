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
typedef struct AssignmentExpression AssignmentExpression_T;
typedef struct ArrayAccessExpression ArrayAccessExpression_T;
typedef struct MemberAccessExpression MemberAccessExpression_T;
typedef struct ExpressionList ExpressionList_T;

enum ExpressionType {
    EXPRESSION_TYPE_LITERAL,
    EXPRESSION_TYPE_UNARY,
    EXPRESSION_TYPE_BINARY,
    EXPRESSION_TYPE_NESTED,
	EXPRESSION_TYPE_FUNCTIONCALL,
	EXPRESSION_TYPE_ASSIGNMENT,
	EXPRESSION_TYPE_ARRAYACCESS,
	EXPRESSION_TYPE_MEMBERACCESS,
	EXPRESSION_TYPE_LIST,
};

struct Expression {
    ExpressionType type;

    union {
        Literal_T *literal_expr;
        UnaryExpression_T *unary_expr;
        BinaryExpression_T *binary_expr;
        NestedExpression_T *nested_expr;
		FunctionCallExpression_T *func_call_expr;
		AssignmentExpression_T *assignment_expr;
		ArrayAccessExpression_T *array_access_expr;
		MemberAccessExpression_T *member_access_expr;
		ExpressionList_T *list_expr;
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
	ExpressionList_T *argument_expression_list;
	ArrayList *argument_datatypes;
};

struct AssignmentExpression {
	Expression_T *identifier;
	AssignmentOperator_T operator;
	Expression_T *assignment_value;
};

struct MemberAccessExpression {
	Expression_T *identifier;
	char *member_identifier;
};

struct ArrayAccessExpression {
	Literal_T *identifier;
	Expression_T *index_expression;
};

struct ExpressionList {
	ArrayList *expressions;
};

extern const char *EXPRESSION_TYPES[];

void expression_free(Expression_T *expression);

#endif // LUVA_EXPRESSION_H