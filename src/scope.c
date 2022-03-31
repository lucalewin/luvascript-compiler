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
#include <types/import.h>
#include <types/package.h>
#include <types/module.h>

/**
 * @brief evaluates the scopes of the ast
 * 
 * @param ast the ast to evaluate
 */
int scope_evaluate_ast(AST *ast, ArrayList *modules) {
	if (ast == NULL) return 1;

	// evaluate imports
	for (size_t i = 0; i < ast->packages->size; i++) {
		Package *package = arraylist_get(ast->packages, i);
		package->imported_functions = arraylist_create();
		package->imported_global_variables = arraylist_create();

		// log_debug("evaluating imports in package %s\n", package->name);
		for (size_t j = 0; j < package->import_stmts->size; j++) {
			import_stmt_t *import = arraylist_get(package->import_stmts, j);

			if (import->type_identifier == NULL) {
				// log_debug("import statement: %s\n", import->package_name);
				break;
			}

			if (strcmp(import->package_name, package->name) == 0) {
				log_error("import statement cannot import self\n");
				return 1;
			}

			for (size_t k = 0; k < ast->packages->size; k++) {
				Package *imported_package = arraylist_get(ast->packages, k);
				if (strcmp(imported_package->name, import->package_name) != 0) {
					continue;
				}

				// log_debug("import statement: %s\n", import->package_name);

				for (size_t l = 0; l < imported_package->functions->size; l++) {
					Function *function = arraylist_get(imported_package->functions, l);
					if (strcmp(function->identifier, import->type_identifier) == 0) {
						function->is_imported = 1;
						arraylist_add(package->imported_functions, function);
						goto type_imported;
					}
				}

				for (size_t l = 0; l < imported_package->global_variables->size; l++) {
					Variable *var = arraylist_get(imported_package->global_variables, l);
					if (strcmp(var->identifier->value, import->type_identifier) == 0) {
						// function->is_imported = 1;
						arraylist_add(package->imported_global_variables, var);
						goto type_imported;
					}
				}
			}
			
			log_error("import statement: %s\n", import->package_name);
			return 1;

			type_imported: ;
			// log_debug("import type '%s' from package '%s'\n", import->type_identifier, import->package_name);
		}
	}

	for (size_t i = 0; i < ast->packages->size; i++) {
		Package *package = arraylist_get(ast->packages, i);
		if (scope_evaluate_package(package) != 0) {
			log_error("error evaluating package %s\n", package->name);
			return 1;
		}
	}

	return 0;
}

int scope_evaluate_package(Package *package) {
	// log_debug("evaluating scope of package '%s'\n", package->name);

	// initialize a empty global scope
	package->package_scope = scope_new();
	package->package_scope->parent = NULL;

	// convert variables to variable templates + add them to the global scope
	for (size_t i = 0; i < package->global_variables->size; i++) {
		Variable *var = arraylist_get(package->global_variables, i);
		arraylist_add(package->package_scope->global_variable_templates, convert_to_variable_template(var));
	}

	// convert functions to function templates + add them to the global scope
	for (size_t i = 0; i < package->functions->size; i++) {
		Function *func = arraylist_get(package->functions, i);
		FunctionTemplate *func_template = convert_to_function_template(func);
		arraylist_add(package->package_scope->function_templates, func_template);
	}

	// convert imported functions to function templates + add them to the global scope
	for (size_t i = 0; i < package->imported_functions->size; i++) {
		Function *func = arraylist_get(package->imported_functions, i);
		FunctionTemplate *func_template = convert_to_function_template(func);
		arraylist_add(package->package_scope->function_templates, func_template);
	}


	// external functions are already function templates -> add them directly to the global scope
	arraylist_addall(package->package_scope->function_templates, package->extern_functions);

	// evaluate scopes for all functions
	for (size_t i = 0; i < package->functions->size; i++) {
		Function *func = arraylist_get(package->functions, i);
		func->scope = scope_copy(package->package_scope);
		func->scope->parent = package->package_scope;
		if (!scope_evaluate_function(func)) {
			// some error occurred while evaluating the function
			return 1;
		}
	}

	return 0;
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
			log_error("error while evaluating statement\n");
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
		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			// TODO: handle assembly code blocks
			break;
		default:
			log_error("unexpected statement type '%d'\n", stmt->type);
			return 0;
	}

	return 1;
}

// -----------------------------------------------------------

/**
 * @brief creates a new scope and initializes it with default values
 * 
 * @return Scope* the newly allocated memory for the scope
 */
Scope *scope_new() {
	Scope *scope = calloc(1, sizeof(Scope));
	if (scope == NULL) {
		log_error("could not allocate memory for scope\n");
		return NULL;
	}
	scope->global_variable_templates = arraylist_create();
	scope->local_variable_templates = arraylist_create();
	scope->function_templates = arraylist_create();
	return scope;
}

/**
 * @brief creates a new scope and copies all variables and functions from the given scope
 * 
 * @param scope the scope to be copied
 * @return Scope* the new scope
 */
Scope *scope_copy(Scope *scope) {
	Scope *copy = calloc(1, sizeof(Scope));
	copy->global_variable_templates = arraylist_copy(scope->global_variable_templates);
	copy->local_variable_templates = arraylist_copy(scope->local_variable_templates);
	copy->function_templates = arraylist_copy(scope->function_templates);
	return copy;
}

/**
 * @brief joins two scopes together
 * 
 * @param scope the first scope
 * @param other the second scope
 * @return Scope* the joined scope
 */
Scope *scope_join(Scope *scope, Scope *other) {
	Scope *joined = scope_copy(scope);

	arraylist_addall(joined->global_variable_templates, other->global_variable_templates);
	arraylist_addall(joined->local_variable_templates, other->local_variable_templates);
	arraylist_addall(joined->function_templates, other->function_templates);

	return joined;
}

/**
 * @brief frees the memory allocated memory for the given scope
 * 
 * @param scope the scope to be freed
 */
void scope_free(Scope *scope) {
	if (scope == NULL) {
		return;
	}
	arraylist_free(scope->global_variable_templates);
	arraylist_free(scope->local_variable_templates);
	arraylist_free(scope->function_templates);
	free(scope);
}

void scope_merge(Scope *dest, Scope *src) {
	if (src == NULL) return;
	if (dest == NULL) return;
	for (size_t i = 0; i < src->global_variable_templates->size; i++) {
		arraylist_add(dest->global_variable_templates, arraylist_get(src->global_variable_templates, i));
	}
	for (size_t i = 0; i < src->local_variable_templates->size; i++) {
		arraylist_add(dest->local_variable_templates, arraylist_get(src->local_variable_templates, i));
	}
	for (size_t i = 0; i < src->function_templates->size; i++) {
		arraylist_add(dest->function_templates, arraylist_get(src->function_templates, i));
	}
	if (dest->parent != NULL && src->parent != NULL) {
		scope_merge(dest->parent, src->parent);
	}
	// arraylist_addall(dest->global_variable_templates, src->global_variable_templates);
	// arraylist_addall(dest->local_variable_templates, src->local_variable_templates);
	// arraylist_addall(dest->function_templates, src->function_templates);
}

/**
 * @brief calculates the offset of the given variable on the stack
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int the offset of the variable on the stack
 */
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

/**
 * @brief checks if the given scope contains a local or global variable with the given name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int 1 if the variable is defined, 0 otherwise
 */
int scope_contains_variable(Scope *scope, char *var_name) {
	return scope_contains_local_variable(scope, var_name) || scope_contains_global_variable(scope, var_name);
}

/**
 * @brief checks if the given scope contains a local variable with the given name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int 1 if the variable is defined localy, 0 otherwise
 */
int scope_contains_local_variable(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *local_variable_template = arraylist_get(scope->local_variable_templates, i);
		if (strcmp(local_variable_template->identifier, var_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

/**
 * @brief checks if the given scope contains a global variable with the given name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int 1 if the variable is defined globaly, 0 otherwise
 */
int scope_contains_global_variable(Scope *scope, char *var_name) {
	for (int i = 0; i < scope->global_variable_templates->size; i++) {
		VariableTemplate *global_variable_template = arraylist_get(scope->global_variable_templates, i);
		if (strcmp(global_variable_template->identifier, var_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

/**
 * @brief get the address of the variable with the given name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return char* the address of the variable
 */
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

/**
 * @brief checks if the given scope contains a function with the given name
 * 
 * @param scope the current scope
 * @param func_name the name of the function
 * @return int 1 if the function is defined, 0 otherwise
 */
int scope_contains_function(Scope *scope, char *func_name) {
	for (int i = 0; i < scope->function_templates->size; i++) {
		FunctionTemplate *func_template = arraylist_get(scope->function_templates, i);
		if (strcmp(func_template->identifier, func_name) == 0) {
			return 1; // true
		}
	}
	return 0; // false
}

/**
 * @brief get the variable template of the variable with the given name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return VariableTemplate* the variable template of the variable
 */
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

/**
 * @brief get the function template of the function with the given name
 * 
 * @param scope the current scope
 * @param func_name the name of the function
 * @return FunctionTemplate* the function template of the function
 */
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
