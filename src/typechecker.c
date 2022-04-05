#include <typechecker.h>

#include <types/ast.h>
#include <types/package.h>
#include <types/function.h>
#include <types/statement.h>
#include <types/variable.h>

#include <logging/logger.h>
#include <logging/debug.h>

/**
 * @brief checks if the given ast contains duplicate functions
 * 
 * @param ast the current ast
 * @return true if duplicate functions were found, false otherwise
 */
bool duplicate_functions_exist(AST *ast);

/**
 * @brief checks if the given ast contains duplicate variables
 * 
 * @param ast the current ast
 * @return true if duplicate variables were found, false otherwise
 */
bool duplicate_variables_exist(AST *ast);

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool all_code_paths_return(AST *ast);

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool all_code_blocks_are_reachable(AST *ast);

// utility functions

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param conditional_stmt 
 * @return true 
 */
bool check_if_branch_retuns(ConditionalStatement *conditional_stmt);

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param conditional_stmt 
 * @return true 
 */
bool branch_contains_unreachable_code(ConditionalStatement *conditional_stmt);

// ----------------------------------------------------------------

bool check_types(AST *ast) {
	log_info("Starting type-checking...\n");

	if (duplicate_functions_exist(ast)) {
		return false;
	}

	if (duplicate_variables_exist(ast)) {
		return false;
	}

	if (!all_code_paths_return(ast)) {
		return false;
	}

	if (!all_code_blocks_are_reachable(ast)) {
		// warning is already printed
	}

	log_info("Finnished type-checking\n");
	return true;
}

// ----------------------------------------------------------------
// implementation
// ----------------------------------------------------------------

bool duplicate_functions_exist(AST *ast) {
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

bool duplicate_variables_exist(AST *ast) {
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

bool all_code_paths_return(AST *ast) {
	log_debug("Checking if all code paths return\n");

	bool all_code_paths_return = true;

	for (int i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		for (int j = 0; j < arraylist_size(package->functions); j++) {
			Function *function = arraylist_get(package->functions, j);

			if (arraylist_size(function->body_statements) == 0) {
				printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) has an empty body\n", 
						function->identifier,
						"TODO", 0, 0);
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

			// if the function returns `void` then no return statement is required
			// but internally we need to have a return statement, otherwise a
			// `SEGFAULT` occurs when the function is called
			if (strcmp(function->return_type->type_identifier, "void") == 0) {

				Statement *statement = calloc(1, sizeof(Statement));
				statement->scope = function->scope;
				statement->type = STATEMENT_RETURN;
				statement->stmt.return_statement = calloc(1, sizeof(ReturnStatement));
				statement->stmt.return_statement->expression = NULL;

				arraylist_add(function->body_statements, statement);
				goto next_function;
			}

			printf("    " IRED "error: " RESET "function '%s' (%s:%d:%d) does not return\n", 
					function->identifier,
					"TODO", 0, 0);

			all_code_paths_return = false;

			next_function: ;
		}
	}

	return all_code_paths_return;
}

/**
 * If a path cannot be reached, the compiler will
 * remove the unreachable code.
 */
bool all_code_blocks_are_reachable(AST *ast) {
	log_debug("Checking if all code paths are reachable\n");
	// TODO(lucalewin): implement

	bool all_code_paths_return = true;

	for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		for (size_t j = 0; j < arraylist_size(package->functions); j++) {
			Function *function = arraylist_get(package->functions, j);

			for (size_t k = 0; k < arraylist_size(function->body_statements); k++) {
				Statement *statement = arraylist_get(function->body_statements, k);

				if (statement->type == STATEMENT_CONDITIONAL) {
					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

					branch_contains_unreachable_code(conditional_statement);

					if (check_if_branch_retuns(conditional_statement)) {
						// check if more statements exist after the branch
						if (k + 1 < arraylist_size(function->body_statements)) {
							// there are statements after the conditional statement
							// which cannot be reached because every branch of the
							// conditional statement returns
							printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
									function->identifier,
									"TODO", 0, 0);
							
							all_code_paths_return = false;

							// remove unreachable code
							for (size_t l = arraylist_size(function->body_statements) - 1; l > k; l--)
							{
								Statement *statement = arraylist_get(function->body_statements, l);
								statement_free(statement);
								arraylist_remove_at_index(function->body_statements, l);
							}
						}
					}
				} else if (statement->type == STATEMENT_RETURN) {
					// check if more statements exist after the return statement
					if (k + 1 < arraylist_size(function->body_statements)) {
						// there are statements after the return statement
						// which cannot be reached because the function returns
						printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
								function->identifier,
								"TODO", 0, 0);
						
						all_code_paths_return = false;

						// remove unreachable code
						for (size_t l = arraylist_size(function->body_statements) - 1; l > k; l--)
						{
							Statement *statement = arraylist_get(function->body_statements, l);
							statement_free(statement);
							arraylist_remove_at_index(function->body_statements, l);
						}
					}
				} else if (statement->type == STATEMENT_LOOP) {
					// check if loop condition is always true
					// if it is, then check if the loop breaks
					// else the code after 
				}
			}
		}
	}

	return all_code_paths_return;
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
			CompoundStatement *compound_statement = conditional_stmt->false_branch->stmt.compound_statement;

			for (int i = 0; i < arraylist_size(compound_statement->nested_statements); i++) {
				Statement *statement = arraylist_get(compound_statement->nested_statements, i);

				// check if statement is a branch
				if (statement->type == STATEMENT_CONDITIONAL) {
					// check if branch returns
					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

					// TODO: add checks
					// false_branch_retuns = check_if_branch_retuns(conditional_statement);
					if (check_if_branch_retuns(conditional_statement)) {
						false_branch_retuns = true;
						break;
					}
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

bool branch_contains_unreachable_code(ConditionalStatement *conditional_stmt) {

	bool true_branch_contains_unreachable_code = false, false_branch_contains_unreachable_code = false;

	// check if true branch contains unreachable code
	if (conditional_stmt->true_branch->type == STATEMENT_CONDITIONAL) {
		ConditionalStatement *true_branch_conditional_stmt = conditional_stmt->true_branch->stmt.conditional_statement;

		if (branch_contains_unreachable_code(true_branch_conditional_stmt)) {
			true_branch_contains_unreachable_code = true;
		}
	} else if (conditional_stmt->true_branch->type == STATEMENT_COMPOUND) {
		CompoundStatement *compound_statement = conditional_stmt->true_branch->stmt.compound_statement;

		for (size_t k = 0; k < arraylist_size(compound_statement->nested_statements); k++) {
			Statement *statement = arraylist_get(compound_statement->nested_statements, k);

			if (statement->type == STATEMENT_CONDITIONAL) {
				ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

				if (branch_contains_unreachable_code(conditional_statement)) {
					true_branch_contains_unreachable_code = true;
				}

				if (check_if_branch_retuns(conditional_statement)) {
					// check if more statements exist after the branch
					if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
						// there are statements after the conditional statement
						// which cannot be reached because every branch of the
						// conditional statement returns
						printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
								//function->identifier,
								"TODO",
								"TODO", 0, 0);
						
						true_branch_contains_unreachable_code = true;

						// TODO(lucalewin): remove unreachable code
						for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
						{
							Statement *statement = arraylist_get(compound_statement->nested_statements, l);
							statement_free(statement);
							arraylist_remove_at_index(compound_statement->nested_statements, l);
						}
					}
				}
			} else if (statement->type == STATEMENT_RETURN) {
				// check if more statements exist after the return statement
				if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
					// there are statements after the return statement
					// which cannot be reached because the function returns
					printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
							//function->identifier,
							"TODO",
							"TODO", 0, 0);
					
					true_branch_contains_unreachable_code = true;

					// TODO(lucalewin): remove unreachable code
					for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
					{
						Statement *statement = arraylist_get(compound_statement->nested_statements, l);
						statement_free(statement);
						arraylist_remove_at_index(compound_statement->nested_statements, l);
					}
				}
			} else if (statement->type == STATEMENT_LOOP) {
				// TODO(lucalewin): check if loop condition is always true
				// if it is, then check if the loop breaks
				// else the code after 
			}
		}
	} else if (conditional_stmt->true_branch->type == STATEMENT_LOOP) {
		// TODO(lucalewin): check if loop condition is always true
		// if it is, then check if the loop breaks
		// else the code after 
	} // else: statement is not a branch

	// check if false branch contains unreachable code
	// check if false branch exists
	if (conditional_stmt->false_branch != NULL) {
		// check if false branch returns
		if (conditional_stmt->false_branch->type == STATEMENT_RETURN) {
			false_branch_contains_unreachable_code = true;
		} else if (conditional_stmt->false_branch->type == STATEMENT_CONDITIONAL) {
			ConditionalStatement *false_branch_conditional_stmt = conditional_stmt->false_branch->stmt.conditional_statement;

			if (branch_contains_unreachable_code(false_branch_conditional_stmt)) {
				false_branch_contains_unreachable_code = true;
			}
		} else if (conditional_stmt->false_branch->type == STATEMENT_COMPOUND) {
			CompoundStatement *compound_statement = conditional_stmt->false_branch->stmt.compound_statement;

			for (size_t k = 0; k < arraylist_size(compound_statement->nested_statements); k++) {
				Statement *statement = arraylist_get(compound_statement->nested_statements, k);

				if (statement->type == STATEMENT_CONDITIONAL) {
					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

					if (branch_contains_unreachable_code(conditional_statement)) {
						false_branch_contains_unreachable_code = true;
					}

					if (check_if_branch_retuns(conditional_statement)) {
						// check if more statements exist after the branch
						if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
							// there are statements after the conditional statement
							// which cannot be reached because every branch of the
							// conditional statement returns
							printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
									//function->identifier,
									"TODO",
									"TODO", 0, 0);
							
							false_branch_contains_unreachable_code = true;

							// TODO(lucalewin): remove unreachable code
							for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
							{
								Statement *statement = arraylist_get(compound_statement->nested_statements, l);
								statement_free(statement);
								arraylist_remove_at_index(compound_statement->nested_statements, l);
							}
						}
					}
				} else if (statement->type == STATEMENT_RETURN) {
					// check if more statements exist after the return statement
					if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
						// there are statements after the return statement
						// which cannot be reached because the function returns
						printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
								//function->identifier,
								"TODO",
								"TODO", 0, 0);
						
						false_branch_contains_unreachable_code = true;

						// TODO(lucalewin): remove unreachable code
						for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
						{
							Statement *statement = arraylist_get(compound_statement->nested_statements, l);
							statement_free(statement);
							arraylist_remove_at_index(compound_statement->nested_statements, l);
						}
					}
				} else if (statement->type == STATEMENT_LOOP) {
					// TODO(lucalewin): check if loop condition is always true
					// if it is, then check if the loop breaks
					// else the code after 
				}
			}
		} else if (conditional_stmt->false_branch->type == STATEMENT_LOOP) {
			// TODO(lucalewin): check if loop condition is always true
			// if it is, then check if the loop breaks
			// else the code after 
		} // else: statement is not a branch
	}

	return true_branch_contains_unreachable_code || false_branch_contains_unreachable_code;
}
