#include <scope.h>
#include <scope_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>

#include <types/variable.h>
#include <types/statement.h>
#include <types/function.h>

void scope_evaluate_ast(AST *ast) {
	// initialize a empty global scope
	ast->global_scope = scope_new();

	for (int i = 0; i < ast->functions->size; i++) {
		Function *func = arraylist_get(ast->functions, i);
		arraylist_add(ast->global_scope->functions, func);
	}

	for (int i = 0; i < ast->functions->size; i++) {
		Function *func = arraylist_get(ast->functions, i);
		func->global_scope = ast->global_scope;
		scope_evaluate_function(func);
	}
}

void scope_evaluate_function(Function *function) {
	// log_debug("scope_evaluate_function(): evaluating scope of function '%s'\n", function->identifier);

	function->local_scope = scope_new();

	// add function params to local scope
	for (size_t i = 0; i < function->parameters->size; i++) {
		Variable *parameter = arraylist_get(function->parameters, i);
		
		if (scope_contains_variable_name(function->local_scope, parameter->identifier->value)) {
			log_error("variable already defined\n");
			exit(1);
		}

		scope_add_variable(function->local_scope, parameter);
	}

	// evaluate scope of statements
	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *stmt = arraylist_get(function->body_statements, i);

		stmt->parent_scope = function->local_scope;

		Scope *stmt_scope = scope_new();
		stmt_scope->functions = function->global_scope->functions;
		stmt_scope->variables = arraylist_create();
		arraylist_addall(stmt_scope->variables, function->global_scope->variables);
		arraylist_addall(stmt_scope->variables, function->local_scope->variables);

		stmt->local_scope = stmt_scope;

		scope_evaluate_statement(stmt);
	}

	// log_debug("scope_evaluate_function(): finished scope evaluation of function '%s'\n", function->identifier);
}

void scope_evaluate_statement(Statement *stmt) {
	switch (stmt->type) {
		case STATEMENT_COMPOUND:
			// TODO
			break;

		case STATEMENT_EXPRESSION:
			break;

		case STATEMENT_RETURN:
			break;

		case STATEMENT_VARIABLE_DECLARATION: {
			// log_debug("scope_evaluate_statement(): evaluating variable declaration statement\n");

			Variable *var = stmt->stmt.variable_decl->var;

			if (scope_contains_variable_name(stmt->local_scope, var->identifier->value)) {
				log_error("variable '%s' is already defined!\n", var->identifier->value);
				// TODO: free memory
				exit(1);
			}

			scope_add_variable(stmt->parent_scope, var);

			break;
		}

		default:
			log_error("unexpected statement type '%d'\n", stmt->type);
			exit(1);
	}
}

// -----------------------------------------------------------

Scope *scope_new() {
	Scope *scope = calloc(1, sizeof(Scope));
	scope->variables = arraylist_create();
	scope->functions = arraylist_create();
	scope->function_templates = arraylist_create();
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

int scope_contains_function_name(Scope *scope, char *func_name) {
	for (int i = 0; i < scope->functions->size; i++) {
		Function *function = arraylist_get(scope->functions, i);
		if (strcmp(func_name, function->identifier) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

void scope_add_variable(Scope *scope, Variable *var) {
	arraylist_add(scope->variables, var);
}

void scope_add_function(Scope *scope, Function *function) {
	arraylist_add(scope->functions, function);
}

void scope_remove_variable(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->variables->size; i++) {
		Variable *var = arraylist_get(scope->variables, i);
		if (strcmp(var_name, var->identifier->value) == 0) {
			arraylist_remove_at_index(scope->variables, i);
			return;
		}
	}
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

Function *scope_get_function_by_name(Scope *scope, char *func_name) {
	for (int i = 0; i < scope->functions->size; i++) {
		Function *func = arraylist_get(scope->functions, i);
		if (strcmp(func_name, func->identifier) == 0) {
			return func;
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
		// offset += var->datatype_old / 8;
		offset += var->datatype->size;
	}
	// in bytes 0..7 the old value of rbp is store -> add 8 bytes to avoid overwriting old value of rbp
	return offset + 8;
}
