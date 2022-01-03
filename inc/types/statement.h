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
		CompoundStatement *compound_statement;
		ExpressionStatement *expression_statement;
		ReturnStatement *return_statement;
		VariableDeclarationStatement *variable_decl;
		ConditionalStatement *conditional_statement;
		LoopStatement *loop_statement;
		AssemblyCodeBlockStatement *assembly_code_block_statement;
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
	Expression_T *expression;
};

struct VariableDeclarationStatement {
	Variable *variable;
};

struct ConditionalStatement {
	Expression_T *condition;
	Statement *true_branch;
	Statement *false_branch;
};

struct LoopStatement {
	Expression_T *condition;
	Statement *body;
};

struct AssemblyCodeBlockStatement {
	char *code;
};

extern const char* STATEMENT_TYPES[];

void statement_free(Statement *statement);

#endif // LUVA_STATEMENT_H