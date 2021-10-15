#ifndef LUVA_OPERATOR_H
#define LUVA_OPERATOR_H

typedef enum UnaryOperator {
    UNARY_OPERATOR_PRE_INCREMENT,
    // ...
    // TODO: add missing unary operator types
} UnaryOperator_T;

typedef enum BinaryOperator {
    BINARY_OPERATOR_PLUS,
    BINARY_OPERATOR_MINUS,
    BINARY_OPERATOR_MULTIPLY,
    BINARY_OPERATOR_DIVIDE
    // ...
    // TODO: add missing binary operator types
} BinaryOperator;

typedef enum AssignmentOperator {
    ASSIGNMENT_OPERATOR_DEFAULT,
    ASSIGNMENT_OPERATOR_ADD,
    ASSIGNMENT_OPERATOR_SUBTRACT,
    ASSIGNMENT_OPERATOR_MULTIPLY,
    ASSIGNMENT_OPERATOR_DIVIDE
    // ... 
    // TODO: add missing assignment operator types
} AssignmentOperator_T;

#endif