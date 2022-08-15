#ifndef LUVA_EXPRESSION_H
#define LUVA_EXPRESSION_H

#include <types/datatype.h>
#include <parsing/nodes/operator.h>
#include <parsing/nodes/literal.h>
#include <util/arraylist.h>

typedef enum ExpressionType ExpressionType;

typedef struct _expression                 Expression;
typedef struct _unary_expression           UnaryExpression;
typedef struct _binary_expression          BinaryExpression;
typedef struct _function_call_expression   FunctionCallExpression;
typedef struct _assignment_expression      AssignmentExpression;
typedef struct _array_access_expression    ArrayAccessExpression;
typedef struct _member_access_expression   MemberAccessExpression;
typedef struct _expression_list            ExpressionList;

enum ExpressionType {
    EXPRESSION_TYPE_LITERAL,
    EXPRESSION_TYPE_UNARY,
    EXPRESSION_TYPE_BINARY,
    EXPRESSION_TYPE_NESTED,
	EXPRESSION_TYPE_FUNCTION_CALL,
	EXPRESSION_TYPE_ASSIGNMENT,
	EXPRESSION_TYPE_ARRAY_ACCESS,
	EXPRESSION_TYPE_MEMBER_ACCESS,
	EXPRESSION_TYPE_LIST,
};

struct _expression {
    ExpressionType type;

    union {
        Literal *literal;
        UnaryExpression *unary;
        BinaryExpression *binary;
		Expression *nested;
		FunctionCallExpression *function_call;
		AssignmentExpression *assignment;
		ArrayAccessExpression *array_access;
		MemberAccessExpression *member_access;
		ExpressionList *list;
    } expr;
};

struct _unary_expression {
    Literal *identifier;
    UnaryOperator operator;
	DataType *datatype;
};

struct _binary_expression {
    Expression *left;
    BinaryOperator operator;
    Expression *right;
	DataType *datatype;
};

struct _function_call_expression {
	char *function_identifier;
	unsigned long long int id;
	ExpressionList *argument_expression_list;
	ArrayList *argument_datatypes;
	DataType *datatype;
};

struct _assignment_expression {
	Expression *identifier;
	AssignmentOperator operator;
	Expression *assignment_value;
	DataType *datatype;
};

struct _member_access_expression {
	// char *identifier;
	Expression *identifier;
	char *member_identifier;
	DataType *datatype;
};

struct _array_access_expression {
	Literal *identifier;
	Expression *index_expression;
	DataType *datatype;
};

struct _expression_list {
	ArrayList *expressions;
	DataType *datatype;
};

extern const char *EXPRESSION_TYPES[];

Expression *expression_new(ExpressionType type);
void expression_free(Expression *expression);

DataType *expression_get_datatype(Expression *expression);

#endif // LUVA_EXPRESSION_H