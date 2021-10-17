#ifndef LUVA_EXPRESSION_H
#define LUVA_EXPRESSION_H

#include <types/operator.h>
#include <types/literal.h>

typedef enum ExpressionType ExpressionType;
typedef struct Expression Expression_T;
typedef struct UnaryExpression UnaryExpression_T;
typedef struct BinaryExpression BinaryExpression_T;
typedef struct NestedExpression NestedExpression_T;

enum ExpressionType {
    EXPRESSION_LITERAL,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY,
    EXPRESSION_NESTED
};

struct Expression {
    ExpressionType type;

    union {
        Literal_T *literal_expr;
        UnaryExpression_T *unary_expr;
        BinaryExpression_T *binary_expr;
        NestedExpression_T *nested_expr;
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

extern const char *EXPRESSION_TYPES[];

#endif // LUVA_EXPRESSION_H