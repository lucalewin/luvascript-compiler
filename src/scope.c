#include <scope.h>
#include <scope_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>

void eval_ast(AST *ast) {
	// initialize a empty global scope
	ast->global_scope = scope_new();

	// evaluate statement scopes
	eval_statement(ast->statement, ast->global_scope);
}

void eval_statement(Statement *stmt, Scope *parent_scope) {
	stmt->parent_scope = scope_copy(parent_scope);

	stmt->local_scope = scope_new();

	switch (stmt->type) {
		case STATEMENT_COMPOUND: {
			CompoundStatement *compound_stmt = stmt->stmt.compound_statement;
			
			for (int i = 0; i < compound_stmt->nested_statements->size; i++) {
				Statement *nested_stmt = arraylist_get(compound_stmt->nested_statements, i);

				nested_stmt->parent_scope = stmt->local_scope;
				nested_stmt->local_scope = scope_new();

				switch (nested_stmt->type) {
					case STATEMENT_VARIABLE_DECLARATION: {
						Variable *var = nested_stmt->stmt.variable_decl->var;

						if (scope_contains_variable_name(stmt->local_scope, var->identifier->value)) {
							log_error("variable '%s' is already defined!\n", var->identifier->value);
							// TODO: free memory
							exit(1);
						}

						// TODO: if variable name exists in parent_scope overwrite it in local_scope
						scope_add_variable_name(stmt->local_scope, var);

						break;
					}

					case STATEMENT_RETURN:
					case STATEMENT_EXPRESSION:
					case STATEMENT_COMPOUND: {
						break;
					}

					default:
						log_error("unknown statement type '%d'", nested_stmt->type);
						break;
				}
			}
			break;
		}

		default:
			break;
	}
}

// -----------------------------------------------------------

Scope *scope_new() {
	Scope *scope = calloc(1, sizeof(Scope));
	scope->variables = arraylist_create();
	return scope;
}

Scope *scope_copy(Scope *scope) {
	Scope *copy = calloc(1, sizeof(Scope));
	copy->variables = arraylist_copy(scope->variables);
	return copy;
}

int scope_contains_variable_name(Scope *scope, char *name) {
	for (int i = 0; i < scope->variables->size; i++) {
		Variable *var = arraylist_get(scope->variables, i);
		if (strcmp(name, var->identifier->value) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

void scope_add_variable_name(Scope *scope, Variable *var) {
	arraylist_add(scope->variables, var);
}

Variable *scope_get_variable_by_name(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->variables->size; i++) {
		Variable *var = arraylist_get(scope->variables, i);
		if (strcmp(var_name, var->identifier->value) == 0) {
			return var;
		}
	}
	return NULL;
}

int scope_get_variable_rbp_offset(Scope *scope, char *var_name) {
	int offset = 0;
	for (int i = 0; i < scope->variables->size; i++) {
		Variable *var = arraylist_get(scope->variables, i);
		if (strcmp(var_name, var->identifier->value) == 0) {
			break;
		}
		offset += var->datatype / 8;
	}
	return offset;
}
