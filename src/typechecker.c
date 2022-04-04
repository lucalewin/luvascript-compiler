#include <typechecker.h>

#include <types/ast.h>
#include <types/package.h>
#include <types/function.h>
#include <types/statement.h>
#include <types/variable.h>

#include <logging/logger.h>

/**
 * @brief checks if the given ast contains duplicate functions
 * 
 * @param ast the current ast
 * @return true if duplicate functions were found, false otherwise
 */
bool check_for_duplicate_functions(AST *ast);

/**
 * @brief checks if the given ast contains duplicate variables
 * 
 * @param ast the current ast
 * @return true if duplicate variables were found, false otherwise
 */
bool check_for_duplicate_variables(AST *ast);

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool check_if_all_code_paths_return(AST *ast);

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool check_if_all_code_blocks_are_reachable(AST *ast);

// utility functions

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param conditional_stmt 
 * @return true 
 */
bool check_if_branch_retuns(ConditionalStatement *conditional_stmt);

// ----------------------------------------------------------------

bool check_types(AST *ast) {
	log_info("Starting type-checking...\n");

	if (check_for_duplicate_functions(ast)) {
		return false;
	}

	if (check_for_duplicate_variables(ast)) {
		return false;
	}

	if (!check_if_all_code_paths_return(ast)) {
		return false;
	}

	log_info("Finnished type-checking\n");
	return true;
}

// ----------------------------------------------------------------
// implementation
// ----------------------------------------------------------------

bool check_for_duplicate_functions(AST *ast) {
	log_debug("Checking for duplicate functions\n");

	bool duplicated_functions_found = false;

	for (int i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		for (int j = 0; j < arraylist_size(package->functions); j++) {
			Function *function = arraylist_get(package->functions, j);

			for (int k = j + 1; k < arraylist_size(package->functions); k++) {
				Function *other_function = arraylist_get(package->functions, k);

				// no need to compare the same function
				if (function == other_function) {
					continue;
				}

				// check if functions have the same name
				if (strcmp(function->identifier, other_function->identifier) == 0) {
					// check parameter types

					// first check if parameter counts match
					if (arraylist_size(function->parameters) == arraylist_size(other_function->parameters)) {
						// both functions have the same number of parameters
						// check if parameter types match
						for (int l = 0; l < arraylist_size(function->parameters); l++) {
							Variable *parameter = arraylist_get(function->parameters, l);
							Variable *other_parameter = arraylist_get(other_function->parameters, l);

							// check if type identifier of the parameters match
							if (strcmp(parameter->datatype->type_identifier, other_parameter->datatype->type_identifier) != 0) {
								// parameters have different types
								// therefore functions are not the same
								goto next_other_function;
							}
						}

						printf("    " IRED "error: " RESET "duplicate function '%s' (%s:%d:%d) already defined here (%s:%d:%d)\n", 
								function->identifier,
								"TODO", 0, 0,
								"TODO", 0, 0);

						duplicated_functions_found = true;
						goto next_function;
					}
					// else: parameter count does not match, so we can't compare parameter types
					// and therefore these functions are not equal
				}
				next_other_function: ;
			}
			next_function: ;
		}
	}

	return duplicated_functions_found;
}

bool check_for_duplicate_variables(AST *ast) {
	log_debug("Checking for duplicate variables\n");

	bool duplicated_variables_found = false;

	for (int i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		for (int j = 0; j < arraylist_size(package->global_variables); j++) {
			Variable *variable = arraylist_get(package->global_variables, j);

			for (int k = j + 1; k < arraylist_size(package->global_variables); k++) {
				Variable *other_variable = arraylist_get(package->global_variables, k);

				// no need to compare the same variable
				if (variable == other_variable) {
					continue;
				}

				// check if variables have the same name
				if (strcmp(variable->identifier->value, other_variable->identifier->value) == 0) {
					printf("    " IRED "error: " RESET "duplicate variable '%s' (%s:%d:%d) already defined here (%s:%d:%d)\n", 
							variable->identifier->value,
							"TODO", 0, 0,
							"TODO", 0, 0);

					duplicated_variables_found = true;
					goto next_variable;
				}
			}
			next_variable: ;
		}
	}

	return duplicated_variables_found;
}

bool check_if_all_code_paths_return(AST *ast) {
	log_debug("Checking if all code paths return\n");

	bool all_code_paths_return = true;

	for (int i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		for (int j = 0; j < arraylist_size(package->functions); j++) {
			Function *function = arraylist_get(package->functions, j);

			// if the function returns `void` then no return statement is required
			if (strcmp(function->return_type->type_identifier, "void") == 0) {
				continue;
			}

			// loop through all statements in the function
			// and check if 
			for (int i = 0; i < arraylist_size(function->body_statements); i++) {
				Statement *statement = arraylist_get(function->body_statements, i);

				// check if statement is a branch
				if (statement->type == STATEMENT_CONDITIONAL) {
					// check if branch returns
					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

					// TODO: add checks
					if(check_if_branch_retuns(conditional_statement)) {
						// all_code_paths_return = true;
						goto next_function;
					}
				}

				// check if statement is a return statement
				if (statement->type == STATEMENT_RETURN) {
					// all_code_paths_return = true;
					goto next_function;
				}

			}
			// if this is reached, not all code paths return
			printf("    " IRED "error: " RESET "function '%s' (%s:%d:%d) does not return\n", 
					function->identifier,
					"TODO", 0, 0);

			all_code_paths_return = false;

			next_function: ;
		}
	}

	return all_code_paths_return;
}

bool check_if_all_code_blocks_are_reachable(AST *ast) {
	

	return false;
}

// ----------------------------------------------------------------
// utility functions
// ----------------------------------------------------------------

bool check_if_branch_retuns(ConditionalStatement *conditional_stmt) {
	bool true_branch_retuns = false, false_branch_retuns = false;

	// check if true branch returns
	if (conditional_stmt->true_branch->type == STATEMENT_RETURN) {
		true_branch_retuns = true;
	} else if (conditional_stmt->true_branch->type == STATEMENT_CONDITIONAL) {
		true_branch_retuns = check_if_branch_retuns(conditional_stmt->true_branch->stmt.conditional_statement);
	} else if (conditional_stmt->true_branch->type == STATEMENT_COMPOUND) {
		CompoundStatement *compound_statement = conditional_stmt->true_branch->stmt.compound_statement;

		for (int i = 0; i < arraylist_size(compound_statement->nested_statements); i++) {
			Statement *statement = arraylist_get(compound_statement->nested_statements, i);

			// check if statement is a branch
			if (statement->type == STATEMENT_CONDITIONAL) {
				// check if branch returns
				ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

				// TODO: add checks
				true_branch_retuns = check_if_branch_retuns(conditional_statement);
				break;
			}

			// check if statement is a return statement
			if (statement->type == STATEMENT_RETURN) {
				true_branch_retuns = true;
				break;
			}
		}
	} // else: statement is not a return statement

	// check if false branch exists
	if (conditional_stmt->false_branch != NULL) {
		// check if false branch returns
		if (conditional_stmt->false_branch->type == STATEMENT_RETURN) {
			false_branch_retuns = true;
		} else if (conditional_stmt->false_branch->type == STATEMENT_CONDITIONAL) {
			false_branch_retuns = check_if_branch_retuns(conditional_stmt->false_branch->stmt.conditional_statement);
		} else if (conditional_stmt->false_branch->type == STATEMENT_COMPOUND) {
			CompoundStatement *compound_statement = conditional_stmt->true_branch->stmt.compound_statement;

			for (int i = 0; i < arraylist_size(compound_statement->nested_statements); i++) {
				Statement *statement = arraylist_get(compound_statement->nested_statements, i);

				// check if statement is a branch
				if (statement->type == STATEMENT_CONDITIONAL) {
					// check if branch returns
					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

					// TODO: add checks
					false_branch_retuns = check_if_branch_retuns(conditional_statement);
					break;
				}

				// check if statement is a return statement
				if (statement->type == STATEMENT_RETURN) {
					false_branch_retuns = true;
					break;
				}
			}
		} // else: statement is not a return statement
	}

	return true_branch_retuns && false_branch_retuns;
}
