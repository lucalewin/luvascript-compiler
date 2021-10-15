#ifndef LUVA_STATEMENT_H
#define LUVA_STATEMENT_H

// ---------------------------------

#include <expression.h>
#include <arraylist.h>

// ---------------------------------

typedef struct CompoundStatement CompoundStatement;
typedef struct ExpressionStatement ExpressionStatement;
typedef struct ReturnStatement ReturnStatement;

// ---------------------------------

typedef enum {
    STATEMENT_COMPOUND,
    STATEMENT_EXPRESSION,
    STATEMENT_RETURN
} statement_type;

typedef struct statement {
    statement_type type;

    union {
        CompoundStatement *compound_statement;
        ExpressionStatement *expression_statement;
        ReturnStatement *return_statement;
    } stmt;
} Statement;

struct CompoundStatement {
    ArrayList *nested_statements;
};

struct ExpressionStatement {
    Expression_T *expression;
};

struct ReturnStatement {
    Expression_T *return_expression;
};

extern const char* STATEMENT_TYPES[];

#endif // LUVA_STATEMENT_H