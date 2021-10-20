#ifndef LUVA_STATEMENT_H
#define LUVA_STATEMENT_H

// ---------------------------------

#include <util/arraylist.h>

#include <types/expression.h>
#include <types/variable.h>

#include <scope.h>

// ---------------------------------

typedef struct CompoundStatement CompoundStatement;
typedef struct ExpressionStatement ExpressionStatement;
typedef struct ReturnStatement ReturnStatement;
typedef struct VariableDeclarationStatement VariableDeclarationStatement;

// ---------------------------------

typedef enum {
	STATEMENT_COMPOUND,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
	STATEMENT_VARIABLE_DECLARATION
} statement_type;

typedef struct statement {
	statement_type type;

	Scope *parent_scope;
	Scope *local_scope;

	union {
		CompoundStatement *compound_statement;
		ExpressionStatement *expression_statement;
		ReturnStatement *return_statement;
		VariableDeclarationStatement *variable_decl;
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

struct VariableDeclarationStatement {
	Variable *var;
};

extern const char* STATEMENT_TYPES[];

#endif // LUVA_STATEMENT_H