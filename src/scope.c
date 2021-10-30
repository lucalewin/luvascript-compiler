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

void scope_evaluate_ast(AST *ast) {
	// initialize a empty global scope
	ast->global_scope = scope_new();

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
		scope_evaluate_function(func);
	}
}

void scope_evaluate_function(Function *function) {
	log_debug("scope_evaluate_function()\n");

	// add parameter templates to local variables
	for (size_t i = 0; i < function->parameters->size; i++) {
		Variable *parameter = arraylist_get(function->parameters, i);

		if (scope_contains_local_variable(function->scope, parameter->identifier->value)) {
			log_error("variable already defined\n");
			// TODO(lucalewin): free memory
			exit(1);
		}

		arraylist_add(function->scope->local_variable_templates, convert_to_variable_template(parameter));
	}

	// evaluate scopes of function statements
	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *stmt = arraylist_get(function->body_statements, i);
		stmt->scope = scope_copy(function->scope);
		scope_evaluate_statement(stmt, function->scope);
	}
}

// ----------------------------------------------------------------

void scope_evaluate_statement(Statement *stmt, Scope *parent_scope) {
	// log_debug("scope_evaluate_statement()\n");

	switch (stmt->type) {
		case STATEMENT_COMPOUND:
			// TODO(lucalewin)
			break;

		case STATEMENT_EXPRESSION:
			break;

		case STATEMENT_RETURN:
			break;

		case STATEMENT_VARIABLE_DECLARATION: {
			// log_debug("scope_evaluate_statement(): evaluating variable declaration statement\n");

			Variable *var = stmt->stmt.variable_decl->var;

			if (scope_contains_local_variable(stmt->scope, var->identifier->value)) {
				log_error("variable '%s' is already defined!\n", var->identifier->value);
				// TODO(lucalewin): free memory
				exit(1);
			}

			arraylist_add(parent_scope->local_variable_templates, convert_to_variable_template(var));

			break;
		}

		default:
			log_error("unexpected statement type '%d'\n", stmt->type);
			exit(1);
	}
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
	// copy->variables_old = arraylist_copy(scope->variables_old);
	copy->global_variable_templates = arraylist_copy(scope->global_variable_templates);
	copy->local_variable_templates = arraylist_copy(scope->local_variable_templates);
	copy->function_templates = arraylist_copy(scope->function_templates);
	return copy;
}

// TODO(lucalewin) add documentation
int scope_get_variable_rbp_offset(Scope *scope, char *var_name) {
	int offset = 0;
	for (size_t i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(scope->local_variable_templates, i);
		if (strcmp(var_name, var_template->identifier) == 0) {
			break;
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
