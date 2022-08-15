#ifndef LUVA_STATEMENT_H
#define LUVA_STATEMENT_H

// ---------------------------------

#include <util/arraylist.h>

#include <parsing/nodes/expression.h>
#include <parsing/nodes/variable.h>

#include <parsing/scope.h>

// ---------------------------------

typedef struct CompoundStatement CompoundStatement;
typedef struct ExpressionStatement ExpressionStatement;
typedef struct ReturnStatement ReturnStatement;
typedef struct VariableDeclarationStatement VariableDeclarationStatement;
typedef struct ConditionalStatement ConditionalStatement;
typedef struct LoopStatement LoopStatement;
typedef struct AssemblyCodeBlockStatement AssemblyCodeBlockStatement;

// ---------------------------------

typedef enum {
	STATEMENT_COMPOUND,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
	STATEMENT_VARIABLE_DECLARATION,
	STATEMENT_CONDITIONAL,
	STATEMENT_LOOP,
	STATEMENT_ASSEMBLY_CODE_BLOCK
} statement_type;

typedef struct statement {
	statement_type type;
	Scope *scope;
	union {
		CompoundStatement *compound;
		ExpressionStatement *expression;
		ReturnStatement *_return;
		VariableDeclarationStatement *variable_declaration;
		ConditionalStatement *conditional;
		LoopStatement *loop;
		AssemblyCodeBlockStatement *assembly;
	} stmt;
} Statement;

struct CompoundStatement {
	ArrayList *nested_statements;
	Scope *local_scope;
};

struct ExpressionStatement {
	Expression *expression;
};

struct ReturnStatement {
	Expression *expression;
};

struct VariableDeclarationStatement {
	Variable *variable;
};

struct ConditionalStatement {
	Expression *condition;
	Statement *true_branch;
	Statement *false_branch;
};

struct LoopStatement {
	Expression *condition;
	Statement *body;
};

struct AssemblyCodeBlockStatement {
	char *code;
};

extern const char* STATEMENT_TYPES[];

void statement_free(Statement *statement);

#endif // LUVA_STATEMENT_H