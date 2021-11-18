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
typedef struct ConditionalStatement ConditionalStatement;
typedef struct LoopStatement LoopStatement;

// ---------------------------------

typedef enum {
	STATEMENT_COMPOUND,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
	STATEMENT_VARIABLE_DECLARATION,
	STATEMENT_CONDITIONAL,
	STATEMENT_LOOP
} statement_type;

typedef struct statement {
	statement_type type;
	Scope *scope;
	union {
		CompoundStatement *compound_statement;
		ExpressionStatement *expression_statement;
		ReturnStatement *return_statement;
		VariableDeclarationStatement *variable_decl;
		ConditionalStatement *condtional_statement;
		LoopStatement *loop_statement;
	} stmt;
} Statement;

struct CompoundStatement {
	ArrayList *nested_statements;
	Scope *local_scope;
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

struct ConditionalStatement {
	Expression_T *conditional_expression;

	enum {
		CONDITIONAL_STATEMENT_IF,
		CONDITIONAL_STATEMENT_ELSEIF,
		CONDITIONAL_STATEMENT_ELSE,
	} type;

	Statement *body;
	Statement *else_stmt;
};

struct LoopStatement {
	Expression_T *conditional_expression;
	Statement *body;
};

extern const char* STATEMENT_TYPES[];

#endif // LUVA_STATEMENT_H