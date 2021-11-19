#include <scope.h>
#include <scope_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/util.h>
#include <logging/logger.h>

#include <types/variable.h>
#include <types/statement.h>
#include <types/function.h>

/**
 * @brief evaluates the scopes of the ast
 * 
 * @param ast the ast to evaluate
 */
int scope_evaluate_ast(AST *ast) {
	log_debug("evaluating scope\n");

	// initialize a empty global scope
	ast->global_scope = scope_new();
	ast->global_scope->parent = NULL;

	// convert variables to variable templates + add them to the global scope
	for (size_t i = 0; i < ast->global_variables->size; i++) {
		Variable *var = arraylist_get(ast->global_variables, i);
		arraylist_add(ast->global_scope->global_variable_templates, convert_to_variable_template(var));
	}

	// convert functions to function templates + add them to the global scope
	for (size_t i = 0; i < ast->functions->size; i++) {
		Function *func = arraylist_get(ast->functions, i);

		FunctionTemplate *func_template = calloc(1, sizeof(FunctionTemplate));
		func_template->identifier = func->identifier;
		func_template->return_type = func->return_type;
		func_template->param_datatypes = arraylist_create();

		// convert function parameters to variable templates
		for (size_t j = 0; j < func->parameters->size; j++) {
			Variable *param = arraylist_get(func->parameters, j);
			VariableTemplate *var_template = convert_to_variable_template(param);
			arraylist_add(func_template->param_datatypes, var_template);
		}

		arraylist_add(ast->global_scope->function_templates, func_template);
	}

	// external functions are already function templates -> add them directly to the global scope
	arraylist_addall(ast->global_scope->function_templates, ast->extern_functions);

	// evaluate scopes for all functions
	for (size_t i = 0; i < ast->functions->size; i++) {
		Function *func = arraylist_get(ast->functions, i);
		func->scope = scope_copy(ast->global_scope);
		func->scope->parent = ast->global_scope;
		if (!scope_evaluate_function(func)) {
			// some error occurred while evaluating the function
			return 0;
		}
	}

	return 1;
}

/**
 * @brief evaluates a function's scope
 * 
 * @param function the function to evaluate
 */
int scope_evaluate_function(Function *function) {
	// add parameter templates to local variables
	for (size_t i = 0; i < function->parameters->size; i++) {
		Variable *parameter = arraylist_get(function->parameters, i);
		if (scope_contains_local_variable(function->scope, parameter->identifier->value)) {
			log_error("variable already defined\n");
			return 0;
		}
		arraylist_add(function->scope->local_variable_templates, convert_to_variable_template(parameter));
	}

	// evaluate scopes of function statements
	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *stmt = arraylist_get(function->body_statements, i);

		stmt->scope = scope_copy(function->scope);
		stmt->scope->parent = function->scope;

		if (!scope_evaluate_statement(stmt)) {
			// some error occurred while evaluating the statement
			return 0;
		}
	}

	return 1;
}

// ----------------------------------------------------------------

/**
 * @brief evaluates a statement's scope
 * 
 * @param stmt the statement to evaluate
 */
int scope_evaluate_statement(Statement *stmt) {
	switch (stmt->type) {
		case STATEMENT_COMPOUND: {
			CompoundStatement *compound_stmt = stmt->stmt.compound_statement;

			compound_stmt->local_scope = scope_new();

			for (size_t i = 0; i < compound_stmt->nested_statements->size; i++) {
				Statement *inner_stmt = arraylist_get(compound_stmt->nested_statements, i);

				inner_stmt->scope = scope_join(stmt->scope, compound_stmt->local_scope);
				inner_stmt->scope->parent = compound_stmt->local_scope;

				if (!scope_evaluate_statement(inner_stmt)) {
					// some error occurred while evaluating the statement
					return 0;
				}
			}

			break;
		}
		case STATEMENT_EXPRESSION:
			break;
		case STATEMENT_RETURN:
			break;
		case STATEMENT_VARIABLE_DECLARATION: {
			Variable *var = stmt->stmt.variable_decl->variable;

			if (scope_contains_local_variable(stmt->scope, var->identifier->value)) {
				log_error("variable '%s' is already defined!\n", var->identifier->value);
				return 0;
			}

			arraylist_add(stmt->scope->parent->local_variable_templates, convert_to_variable_template(var));
			
			break;
		}
		case STATEMENT_CONDITIONAL: {
			ConditionalStatement *cond_stmt = stmt->stmt.conditional_statement;

			cond_stmt->true_branch->scope = scope_copy(stmt->scope);
			cond_stmt->true_branch->scope->parent = stmt->scope;

			if (!scope_evaluate_statement(cond_stmt->true_branch)) {
				// some error occurred while evaluating the statement
				return 0;
			}

			if (cond_stmt->false_branch != NULL) {
				cond_stmt->false_branch->scope = scope_copy(stmt->scope);
				cond_stmt->false_branch->scope->parent = stmt->scope;
				if (!scope_evaluate_statement(cond_stmt->false_branch)) {
					// some error occurred while evaluating the statement
					return 0;
				}
			}

			break;
		}
		case STATEMENT_LOOP: {
			LoopStatement *loop_stmt = stmt->stmt.loop_statement;
			
			loop_stmt->body->scope = scope_copy(stmt->scope);
			loop_stmt->body->scope->parent = stmt->scope;

			if (!scope_evaluate_statement(loop_stmt->body)) {
				// some error occurred while evaluating the statement
				return 0;
			}
			break;
		}
		default:
			log_error("unexpected statement type '%d'\n", stmt->type);
			return 0;
	}

	return 1;
}

// -----------------------------------------------------------

// TODO(lucalewin) add documentation
Scope *scope_new() {
	Scope *scope = calloc(1, sizeof(Scope));
	scope->global_variable_templates = arraylist_create();
	scope->local_variable_templates = arraylist_create();
	scope->function_templates = arraylist_create();
	return scope;
}

// TODO(lucalewin) add documentation
Scope *scope_copy(Scope *scope) {
	Scope *copy = calloc(1, sizeof(Scope));
	copy->global_variable_templates = arraylist_copy(scope->global_variable_templates);
	copy->local_variable_templates = arraylist_copy(scope->local_variable_templates);
	copy->function_templates = arraylist_copy(scope->function_templates);
	return copy;
}

Scope *scope_join(Scope *scope, Scope *other) {
	Scope *joined = scope_copy(scope);

	arraylist_addall(joined->global_variable_templates, other->global_variable_templates);
	arraylist_addall(joined->local_variable_templates, other->local_variable_templates);
	arraylist_addall(joined->function_templates, other->function_templates);

	return joined;
}

void scope_free(Scope *scope) {
	arraylist_free(scope->global_variable_templates);
	arraylist_free(scope->local_variable_templates);
	arraylist_free(scope->function_templates);
	free(scope);
}

// TODO(lucalewin) add documentation
int scope_get_variable_rbp_offset(Scope *scope, char *var_name) {
	int offset = 0;
	for (size_t i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(scope->local_variable_templates, i);
		if (strcmp(var_name, var_template->identifier) == 0) {
			return offset + 8;
		}
		offset += var_template->datatype->size;
	}

	// in bytes 0..7 the old value of rbp is store -> add 8 bytes to avoid overwriting old value of rbp
	return offset + 8;
}

// -------------------------------

// TODO(lucalewin) add documentation
int scope_contains_variable(Scope *scope, char *var_name) {
	return scope_contains_local_variable(scope, var_name) || scope_contains_global_variable(scope, var_name);
}

// TODO(lucalewin) add documentation
int scope_contains_local_variable(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *local_variable_template = arraylist_get(scope->local_variable_templates, i);
		if (strcmp(local_variable_template->identifier, var_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

// TODO(lucalewin) add documentation
int scope_contains_global_variable(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->global_variable_templates->size; i++) {
		VariableTemplate *global_variable_template = arraylist_get(scope->global_variable_templates, i);
		if (strcmp(global_variable_template->identifier, var_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

// TODO(lucalewin) add documentation
char *scope_get_variable_address(Scope *scope, char *var_name) {
	if (!scope_contains_variable(scope, var_name)) {
		log_error("undefined variable '%s'\n", var_name);
		return NULL;
	}

	char *address;

	if (scope_contains_local_variable(scope, var_name)) {
		address = straddall("rbp-", int_to_string(scope_get_variable_rbp_offset(scope, var_name)), NULL);
	} else {
		// variable is a global variable
		address = calloc(strlen(var_name) + 1, sizeof(char));
		address = strcpy(address, var_name);
	}

	return address;
}

// TODO(lucalewin) add documentation
int scope_contains_function(Scope *scope, char *func_name) {
	for (int i = 0; i < scope->function_templates->size; i++) {
		FunctionTemplate *func_template = arraylist_get(scope->function_templates, i);
		if (strcmp(func_template->identifier, func_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

// TODO(lucalewin) add documentation
VariableTemplate *scope_get_variable_by_name(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *local_variable_template = arraylist_get(scope->local_variable_templates, i);
		if (strcmp(local_variable_template->identifier, var_name) == 0) {
			return local_variable_template;
		}
	}

	for (int i = 0; i < scope->global_variable_templates->size; i++) {
		VariableTemplate *global_variable_template = arraylist_get(scope->global_variable_templates, i);
		if (strcmp(global_variable_template->identifier, var_name) == 0) {
			return global_variable_template;
		}
	}

	// variable with name $var_name does not exist in the current scope
	return NULL;
}

// TODO(lucalewin) add documentation
FunctionTemplate *scope_get_function_by_name(Scope *scope, char *func_name) {
	for (int i = 0; i < scope->function_templates->size; i++) {
		FunctionTemplate *func_template = arraylist_get(scope->function_templates, i);
		if (strcmp(func_template->identifier, func_name) == 0) {
			return func_template;
		}
	}

	// function with name $func_name does not exist in the current scope
	return NULL;
}
