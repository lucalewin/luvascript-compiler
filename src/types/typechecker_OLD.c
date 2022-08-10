// #include <types/typechecker.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <limits.h>

// #include <parsing/scope.h>
// #include <parsing/scope_impl.h>

// #include <parsing/nodes/ast.h>
// #include <parsing/nodes/package.h>
// #include <parsing/nodes/function.h>
// #include <parsing/nodes/statement.h>
// #include <parsing/nodes/variable.h>
// #include <parsing/nodes/expression.h>
// #include <parsing/nodes/datatypes.h>
// #include <parsing/nodes/operator.h>
// #include <parsing/nodes/enum.h>

// #include <util/logging/logger.h>
// #include <util/logging/debug.h>

// #include <types/datatype.h>
// #include <types/table.h>

// // validate code
// DataTypeTable *table;

// /**
//  * @brief checks if the given ast contains duplicate functions
//  * 
//  * @param ast the current ast
//  * @return true if duplicate functions were found, false otherwise
//  */
// bool duplicate_functions_exist(AST *ast);

// /**
//  * @brief checks if the given ast contains duplicate variables
//  * 
//  * @param ast the current ast
//  * @return true if duplicate variables were found, false otherwise
//  */
// bool duplicate_variables_exist(AST *ast);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param ast 
//  * @return true 
//  */
// bool all_code_paths_return(AST *ast);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param ast 
//  * @return true 
//  */
// bool all_code_blocks_are_reachable(AST *ast);

// // type checking

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param ast 
//  * @return true 
//  * @return false 
//  */
// bool all_types_match(AST *ast);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param ast 
//  * @return true  
//  */
// bool unused_functions_exist(AST *ast);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param ast 
//  * @return true  
//  */
// bool unused_variables_exist(AST *ast);

// /**
//  * @brief TODO(lucalewin): add description
//  * 
//  * @param expression 
//  * @param scope
//  * @return DatatypeOLD* 
//  */
// DatatypeOLD *type_of_expression(Expression_T *expression, Scope *scope);

// // utility functions

// // TODO(lucalewin): add descriptions
// DatatypeOLD *type_of_literal_expression(const Literal_T *literal, Scope *scope);
// DatatypeOLD *type_of_unary_expression(const UnaryExpression_T *unary, Scope *scope);
// DatatypeOLD *datatype_of_binary_expression(const BinaryExpression_T *expr, Scope *scope);
// DatatypeOLD *type_of_function_call_expression(FunctionCallExpression_T *expression, Scope *scope);
// DatatypeOLD *type_of_assignment_expression(AssignmentExpression_T *expression, Scope *scope);
// DatatypeOLD *type_of_array_access_expression(ArrayAccessExpression_T *expression, Scope *scope);
// DatatypeOLD *type_of_member_access_expression(MemberAccessExpression_T *expression, Scope *scope);
// DatatypeOLD *type_of_expression_list(ExpressionList_T *expression_list, Scope *scope);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param variable 
//  * @return true  
//  */
// bool variable_types_match(Variable *variable, Scope *scope);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param statement 
//  * @return true 
//  */
// bool statement_types_match(Statement *statement, Function *function);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param conditional_stmt 
//  * @return true 
//  */
// bool check_if_branch_retuns(ConditionalStatement *conditional_stmt);

// /**
//  * @brief TODO(lucalewin) add description
//  * 
//  * @param conditional_stmt 
//  * @return true 
//  */
// bool branch_contains_unreachable_code(ConditionalStatement *conditional_stmt);

// // ----------------------------------------------------------------

// bool check_types(AST *ast) {
// 	log_debug("Starting type-checking...\n");

// 	table = data_type_table_new();

// 	if (duplicate_functions_exist(ast)) {
// 		log_debug(" - checking for duplicate functions " IRED "x" RESET "\n");
// 		data_type_table_free(table);
// 		return false;
// 	}
// 	log_debug(" - checking for duplicate functions " GREEN "✓" RESET "\n");

// 	if (duplicate_variables_exist(ast)) {
// 		log_debug(" - checking for duplicate variables " IRED "x" RESET "\n");
// 		data_type_table_free(table);
// 		return false;
// 	}
// 	log_debug(" - checking for duplicate variables " GREEN "✓" RESET "\n");

// 	if (!all_types_match(ast)) {
// 		log_debug(" - checking types " IRED "x" RESET "\n");
// 		data_type_table_free(table);
// 		return false;
// 	}
// 	log_debug(" - checking types " GREEN "✓" RESET "\n");

// 	if (!all_code_paths_return(ast)) {
// 		log_debug(" - checking if all code paths return " IRED "x" RESET "\n");
// 		data_type_table_free(table);
// 		return false;
// 	}
// 	log_debug(" - checking if all code paths return " GREEN "✓" RESET "\n");

// 	if (!all_code_blocks_are_reachable(ast)) {
// 		// warning is already printed
// 		log_debug(" - checking for unreachable code " IRED "x" RESET "\n");
// 	} else {
// 		log_debug(" - checking for unreachable code " GREEN "✓" RESET "\n");
// 	}

// 	if (!unused_functions_exist(ast)) {
// 		// warning is already printed
// 		log_debug(" - checking for unused functions " IRED "x" RESET "\n");
// 	} else {
// 		log_debug(" - checking for unused functions " GREEN "✓" RESET "\n");
// 	}

// 	if (!unused_variables_exist(ast)) {
// 		// warning is already printed
// 		log_debug(" - checking for unused variables " IRED "x" RESET "\n");
// 	} else {
// 		log_debug(" - checking for unused variables " GREEN "✓" RESET "\n");
// 	}
	
// 	data_type_table_free(table);

// 	return true;
// }

// // ----------------------------------------------------------------
// // implementation
// // ----------------------------------------------------------------

// bool duplicate_functions_exist(AST *ast) {
// 	// log_debug(" - Checking for duplicate functions\n");

// 	bool duplicated_functions_found = false;

// 	for (int i = 0; i < arraylist_size(ast->packages); i++) {
// 		Package *package = arraylist_get(ast->packages, i);

// 		for (int j = 0; j < arraylist_size(package->functions); j++) {
// 			Function *function = arraylist_get(package->functions, j);

// 			for (int k = j + 1; k < arraylist_size(package->functions); k++) {
// 				Function *other_function = arraylist_get(package->functions, k);

// 				// no need to compare the same function
// 				if (function == other_function) {
// 					continue;
// 				}

// 				// check if functions have the same name
// 				if (strcmp(function->identifier, other_function->identifier) == 0) {
// 					// check parameter types

// 					// first check if parameter counts match
// 					if (arraylist_size(function->parameters) == arraylist_size(other_function->parameters)) {
// 						// both functions have the same number of parameters
// 						// check if parameter types match
// 						for (int l = 0; l < arraylist_size(function->parameters); l++) {
// 							Variable *parameter = arraylist_get(function->parameters, l);
// 							Variable *other_parameter = arraylist_get(other_function->parameters, l);

// 							// check if type identifier of the parameters match
// 							if (strcmp(parameter->typeOLD->type_identifier, other_parameter->typeOLD->type_identifier) != 0) {
// 								// parameters have different types
// 								// therefore functions are not the same
// 								goto next_other_function;
// 							}
// 						}

// 						printf("    " IRED "error: " RESET "duplicate function '%s' (%s:%d:%d) already defined here (%s:%d:%d)\n", 
// 								function->identifier,
// 								"TODO", 0, 0,
// 								"TODO", 0, 0);

// 						duplicated_functions_found = true;
// 						goto next_function;
// 					}
// 					// else: parameter count does not match, so we can't compare parameter types
// 					// and therefore these functions are not equal
// 				}
// 				next_other_function: ;
// 			}
// 			next_function: ;
// 		}
// 	}

// 	return duplicated_functions_found;
// }

// bool duplicate_variables_exist(AST *ast) {
// 	// log_debug(" - Checking for duplicate variables\n");

// 	bool duplicated_variables_found = false;

// 	for (int i = 0; i < arraylist_size(ast->packages); i++) {
// 		Package *package = arraylist_get(ast->packages, i);

// 		for (int j = 0; j < arraylist_size(package->global_variables); j++) {
// 			Variable *variable = arraylist_get(package->global_variables, j);

// 			for (int k = j + 1; k < arraylist_size(package->global_variables); k++) {
// 				Variable *other_variable = arraylist_get(package->global_variables, k);

// 				// no need to compare the same variable
// 				if (variable == other_variable) {
// 					continue;
// 				}

// 				// check if variables have the same name
// 				if (strcmp(variable->identifierOLD->value, other_variable->identifierOLD->value) == 0) {
// 					printf("    " IRED "error: " RESET "duplicate variable '%s' (%s:%d:%d) already defined here (%s:%d:%d)\n", 
// 							variable->identifierOLD->value,
// 							"TODO", 0, 0,
// 							"TODO", 0, 0);

// 					duplicated_variables_found = true;
// 					goto next_variable;
// 				}
// 			}
// 			next_variable: ;
// 		}
// 	}

// 	return duplicated_variables_found;
// }

// bool all_code_paths_return(AST *ast) {
// 	// log_debug(" - Checking if all code paths return\n");

// 	bool all_code_paths_return = true;

// 	for (int i = 0; i < arraylist_size(ast->packages); i++) {
// 		Package *package = arraylist_get(ast->packages, i);

// 		for (int j = 0; j < arraylist_size(package->functions); j++) {
// 			Function *function = arraylist_get(package->functions, j);

// 			if (arraylist_size(function->statements) == 0) {
// 				printf("%s:%d:%d: " YELLOW "warning: " RESET "function '%s' has an empty body\n", 
// 						"TODO", 0, 0,
// 						function->identifier);
// 			}

// 			// loop through all statements in the function
// 			// and check if 
// 			for (int i = 0; i < arraylist_size(function->statements); i++) {
// 				Statement *statement = arraylist_get(function->statements, i);

// 				// check if statement is a branch
// 				if (statement->type == STATEMENT_CONDITIONAL) {
// 					// check if branch returns
// 					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 					if(check_if_branch_retuns(conditional_statement)) {
// 						// all_code_paths_return = true;
// 						goto next_function;
// 					}
// 				}

// 				// check if statement is a return statement
// 				if (statement->type == STATEMENT_RETURN) {
// 					// all_code_paths_return = true;
// 					goto next_function;
// 				}

// 			}
// 			// if this is reached, not all code paths return

// 			// if the function returns `void` then no return statement is required
// 			// but internally we need to have a return statement, otherwise a
// 			// `SEGFAULT` occurs when the function is called
// 			if (strcmp(function->return_typeOLD->type_identifier, "void") == 0) {

// 				Statement *statement = calloc(1, sizeof(Statement));
// 				statement->scope = function->scope;
// 				statement->type = STATEMENT_RETURN;
// 				statement->stmt.return_statement = calloc(1, sizeof(ReturnStatement));
// 				statement->stmt.return_statement->expression = NULL;

// 				arraylist_add(function->statements, statement);
// 				goto next_function;
// 			}

// 			printf("%s:%d:%d: " IRED "error: " RESET "function '%s' does not return\n", 
// 					"TODO", 0, 0,
// 					function->identifier);

// 			all_code_paths_return = false;

// 			next_function: ;
// 		}
// 	}

// 	return all_code_paths_return;
// }

// /**
//  * If a path cannot be reached, the compiler will
//  * remove the unreachable code.
//  */
// bool all_code_blocks_are_reachable(AST *ast) {
// 	// log_debug(" - Checking if all code paths are reachable\n");

// 	bool all_code_paths_return = true;

// 	for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
// 		Package *package = arraylist_get(ast->packages, i);

// 		for (size_t j = 0; j < arraylist_size(package->functions); j++) {
// 			Function *function = arraylist_get(package->functions, j);

// 			for (size_t k = 0; k < arraylist_size(function->statements); k++) {
// 				Statement *statement = arraylist_get(function->statements, k);

// 				if (statement->type == STATEMENT_CONDITIONAL) {
// 					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 					branch_contains_unreachable_code(conditional_statement);

// 					if (check_if_branch_retuns(conditional_statement)) {
// 						// check if more statements exist after the branch
// 						if (k + 1 < arraylist_size(function->statements)) {
// 							// there are statements after the conditional statement
// 							// which cannot be reached because every branch of the
// 							// conditional statement returns
// 							printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
// 									function->identifier,
// 									"TODO", 0, 0);
							
// 							all_code_paths_return = false;

// 							// remove unreachable code
// 							for (size_t l = arraylist_size(function->statements) - 1; l > k; l--)
// 							{
// 								Statement *statement = arraylist_get(function->statements, l);
// 								statement_free(statement);
// 								arraylist_remove_at_index(function->statements, l);
// 							}
// 						}
// 					}
// 				} else if (statement->type == STATEMENT_RETURN) {
// 					// check if more statements exist after the return statement
// 					if (k + 1 < arraylist_size(function->statements)) {
// 						// there are statements after the return statement
// 						// which cannot be reached because the function returns
// 						printf("    " YELLOW "warning: " RESET "function '%s' (%s:%d:%d) contains unreachable code\n", 
// 								function->identifier,
// 								"TODO", 0, 0);
						
// 						all_code_paths_return = false;

// 						// remove unreachable code
// 						for (size_t l = arraylist_size(function->statements) - 1; l > k; l--)
// 						{
// 							Statement *statement = arraylist_get(function->statements, l);
// 							statement_free(statement);
// 							arraylist_remove_at_index(function->statements, l);
// 						}
// 					}
// 				} else if (statement->type == STATEMENT_LOOP) {
// 					// check if loop condition is always true
// 					// if it is, then check if the loop breaks
// 					// else the code after 
// 				}
// 			}
// 		}
// 	}

// 	return all_code_paths_return;
// }

// bool unused_functions_exist(AST *ast) {
// 	return false; // TODO(lucalewin): implement
// }

// bool unused_variables_exist(AST *ast) {
// 	return false; // TODO(lucalewin): implement
// }

// // type checks

// bool all_types_match(AST *ast) {
// 	// log_debug(" - Checking if all types match\n");

// 	bool all_types_match = true;

// 	for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
// 		Package *package = arraylist_get(ast->packages, i);

// 		// check if types of global variables match
// 		for (size_t j = 0; j < arraylist_size(package->global_variables); j++) {
// 			Variable *variable = arraylist_get(package->global_variables, j);

// 			if (!variable_types_match(variable, NULL)) { // FIXME scope should not be NULL
// 				all_types_match = false;
// 			}
// 		}

// 		log_debug("all global variables match: %d\n", all_types_match);

// 		// check if types in functions match
// 		for (size_t j = 0; j < arraylist_size(package->functions); j++) {
// 			Function *function = arraylist_get(package->functions, j);

// 			for (size_t k = 0; k < arraylist_size(function->statements); k++) {
// 				Statement *statement = arraylist_get(function->statements, k);

// 				if (!statement_types_match(statement, function)) {
// 					all_types_match = false;
// 				}

// 				log_debug("all statement types match: %d\n", all_types_match);
// 			}
// 		}

// 		log_debug("all function types match: %d\n", all_types_match);
// 	}

// 	return all_types_match;
// }

// DatatypeOLD *type_of_expression(Expression_T *expression, Scope *scope) {
// 	switch(expression->type) {
// 		case EXPRESSION_TYPE_LITERAL: {
// 			Literal_T *literal = expression->expr.literal_expr;
// 			return type_of_literal_expression(literal, scope);
// 		}
// 		case EXPRESSION_TYPE_UNARY: {
// 			UnaryExpression_T *unary = expression->expr.unary_expr;
// 			return type_of_unary_expression(unary, scope);
// 		}
// 		case EXPRESSION_TYPE_BINARY: {
// 			BinaryExpression_T *binary = expression->expr.binary_expr;
// 			return datatype_of_binary_expression(binary, scope);
// 		}
// 		case EXPRESSION_TYPE_NESTED: {
// 			NestedExpression_T *nested = expression->expr.nested_expr;
// 			return type_of_expression(nested->expression, scope);
// 		}
// 		case EXPRESSION_TYPE_FUNCTION_CALL: {
// 			FunctionCallExpression_T *function_call = expression->expr.func_call_expr;
// 			return type_of_function_call_expression(function_call, scope);
// 		}
// 		case EXPRESSION_TYPE_ASSIGNMENT: {
// 			AssignmentExpression_T *assignment = expression->expr.assignment_expr;
// 			return type_of_assignment_expression(assignment, scope);
// 		}
// 		case EXPRESSION_TYPE_ARRAY_ACCESS: {
// 			ArrayAccessExpression_T *array_access = expression->expr.array_access_expr;
// 			return type_of_array_access_expression(array_access, scope);
// 		}
// 		case EXPRESSION_TYPE_MEMBER_ACCESS: {
// 			MemberAccessExpression_T *member_access = expression->expr.member_access_expr;
// 			return type_of_member_access_expression(member_access, scope);
// 		}
// 		case EXPRESSION_TYPE_LIST: {
// 			ExpressionList_T *list = expression->expr.list_expr;
// 			return type_of_expression_list(list, scope);
// 		}
// 	}

// 	return NULL;
// }

// // ----------------------------------------------------------------
// // utility functions
// // ----------------------------------------------------------------

// DatatypeOLD *type_of_literal_expression(const Literal_T *literal, Scope *scope) {
// 	switch(literal->type) {
// 		case LITERAL_NUMBER: {
// 			char *end;
// 			signed long long int number = strtoll(literal->value, &end, 10);

// 			// check size and therefore the type of the number
// 			if (number >= CHAR_MIN && number <= CHAR_MAX) {
// 				return parse_datatype("i8");
// 			} else if (number >= SHRT_MIN && number <= SHRT_MAX) {
// 				return parse_datatype("i16");
// 			} else if (number >= INT_MIN && number <= INT_MAX) {
// 				return parse_datatype("i32");
// 			} else if (number >= LONG_MIN && number <= LONG_MAX) {
// 				return parse_datatype("i64");
// 			} else {
// 				// TODO: handle unsigned numbers
// 				return NULL;
// 			}
// 		}
// 		case LITERAL_STRING: {
// 			return parse_datatype("string");
// 		}
// 		case LITERAL_CHARACTER: {
// 			return parse_datatype("char");
// 		}
// 		case LITERAL_IDENTIFIER: {
// 			if (!scope_contains_variable(scope, literal->value)) {
// 				printf("%s:%d:%d: " IRED "error: " RESET "variable '%s' is not defined\n",
// 						"TODO", 0, 0, literal->value);
// 				return NULL;
// 			}

// 			VariableTemplate *variable = scope_get_variable_by_name(scope, literal->value);

// 			return variable->datatype;
// 		}
// 		case LITERAL_BOOLEAN: {
// 			return parse_datatype("bool");
// 		}
// 	}
// 	printf("	" IRED "error: " RESET "unknown type of literal expression\n");
// 	return NULL;
// }

// DatatypeOLD *type_of_unary_expression(const UnaryExpression_T *unary, Scope *scope) {
// 	DatatypeOLD *dt_expr = type_of_literal_expression(unary->identifier, scope);

// 	if (dt_expr == NULL) {
// 		// error;
// 		return NULL;
// 	}

// 	switch(unary->operator) {
// 		case UNARY_OPERATOR_NEGATE: {
// 			// check if dt_expr can be negated
// 			if (!datatype_is_number(dt_expr)) {
// 				printf("    " IRED "error: " RESET "unary operator '-' cannot be applied to type '%s'\n", dt_expr->type_identifier);
// 				return NULL;
// 			}
// 			return dt_expr;
// 		}
// 		case UNARY_OPERATOR_INCREMENT: {
// 			// check if dt_expr can be incremented
// 			if (!datatype_is_number(dt_expr)) {
// 				printf("    " IRED "error: " RESET "unary operator '++' cannot be applied to type '%s'\n", dt_expr->type_identifier);
// 				return NULL;
// 			}
// 			return dt_expr;
// 		}
// 	}

// 	printf("    " IRED "error: " RESET "unknown type of unary expression\n");
// 	return NULL;
// }

// DatatypeOLD *datatype_of_binary_expression(const BinaryExpression_T *expr, Scope *scope) {
// 	DatatypeOLD *dt_left = type_of_expression(expr->expression_left, scope);
// 	DatatypeOLD *dt_right = type_of_expression(expr->expression_right, scope);

// 	if (dt_left == NULL || dt_right == NULL) {
// 		// error
// 		return NULL;
// 	}

// 	switch(expr->operator) {

// 		case BINARY_OPERATOR_ADD:
// 		case BINARY_OPERATOR_MULTIPLY: {
// 			if (datatype_is_number(dt_left) && datatype_is_number(dt_right)) {
// 				return dt_left;
// 			} else if ((datatype_is_number(dt_left) && strcmp(dt_right->type_identifier, "string") == 0) ||
// 					(datatype_is_number(dt_right) && strcmp(dt_left->type_identifier, "string") == 0)) {
// 				return parse_datatype("string");
// 			} else {
// 				printf("    " IRED "error: " RESET "binary operator '%s' cannot be applied to types '%s' and '%s'\n",
// 						BINARY_OPERATOR_TYPES[expr->operator],
// 						dt_left->type_identifier,
// 						dt_right->type_identifier);
// 				return NULL;
// 			}
// 			return NULL;
// 		}

// 		case BINARY_OPERATOR_SUBTRACT:
// 		case BINARY_OPERATOR_DIVIDE:
// 		case BINARY_OPERATOR_MODULO: {
// 			if (datatype_is_number(dt_left) && datatype_is_number(dt_right)) {
// 				return dt_left;
// 			} else {
// 				printf("    " IRED "error: " RESET "binary operator '%s' cannot be applied to types '%s' and '%s'\n", 
// 						BINARY_OPERATOR_TYPES[expr->operator], 
// 						dt_left->type_identifier,
// 						dt_right->type_identifier);
// 				return NULL;
// 			}
// 			return NULL;
// 		}

// 		case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
// 		case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
// 		case BINARY_OPERATOR_BITWISE_AND:
// 		case BINARY_OPERATOR_BITWISE_XOR:
// 		case BINARY_OPERATOR_BITWISE_OR: {
// 			// check if dt_left and dt_right are numbers
// 			if (!datatype_is_number(dt_left) || !datatype_is_number(dt_right)) {
// 				printf("    " IRED "error: " RESET "binary operator '%s' cannot be applied to types '%s' and '%s'\n",
// 						BINARY_OPERATOR_TYPES[expr->operator], dt_left->type_identifier, dt_right->type_identifier);
// 				return NULL;
// 			}

// 			return dt_left->size > dt_right->size ? dt_left : dt_right;
// 		}

// 		case BINARY_OPERATOR_LOGICAL_EQUAL:
// 		case BINARY_OPERATOR_LOGICAL_NOT_EQUAL: {
// 			// any type can be compared with '==' and '!='
// 			return parse_datatype("bool");
// 		}

// 		case BINARY_OPERATOR_LOGICAL_GREATER:
// 		case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
// 		case BINARY_OPERATOR_LOGICAL_LESS:
// 		case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL: {
// 			// check if dt_left and dt_right are numbers
// 			if (!datatype_is_number(dt_left) || !datatype_is_number(dt_right)) {
// 				printf("    " IRED "error: " RESET "binary operator '%s' cannot be applied to types '%s' and '%s'\n",
// 						BINARY_OPERATOR_TYPES[expr->operator], dt_left->type_identifier, dt_right->type_identifier);
// 				return NULL;
// 			}
// 			return parse_datatype("bool");
// 		}

// 		case BINARY_OPERATOR_LOGICAL_AND:
// 		case BINARY_OPERATOR_LOGICAL_OR: {
// 			// check if both expressions are of type bool
// 			if (strcmp(dt_left->type_identifier, "bool") != 0 || strcmp(dt_right->type_identifier, "bool") != 0) {
// 				printf("    " IRED "error: " RESET "binary operator '%s' cannot be applied to types '%s' and '%s'\n",
// 						BINARY_OPERATOR_TYPES[expr->operator], dt_left->type_identifier, dt_right->type_identifier);
// 				return NULL;
// 			}
// 			return dt_left; // bool
// 		}
// 	}

// 	return NULL;
// }

// DatatypeOLD *type_of_function_call_expression(FunctionCallExpression_T *expression, Scope *scope) {
// 	// check if function exists
// 	if (!scope_contains_function(scope, expression->function_identifier)) {
// 		printf("%s:%d:%d: " IRED "error: " RESET "function '%s' does not exist\n",
// 				"TODO", 0, 0,
// 				expression->function_identifier);
// 		return NULL;
// 	}

// 	ArrayList *arg_types = arraylist_create();

// 	// evaluate types of arguments
// 	for (size_t i = 0; i < arraylist_size(expression->argument_expression_list->expressions); i++) {
// 		Expression_T *arg_expr = arraylist_get(expression->argument_expression_list->expressions, i);
// 		DatatypeOLD *arg_type = type_of_expression(arg_expr, scope);
// 		if (arg_type == NULL) {
// 			for (size_t j = 0; j < arraylist_size(arg_types); j++) {
// 				DatatypeOLD *dt = arraylist_get(arg_types, j);
// 				datatype_free(dt);
// 			}
// 			arraylist_free(arg_types);
// 			return NULL;
// 		}
// 		arraylist_add(arg_types, arg_type);
// 	}

// 	// loop through all functions
// 	for (size_t i = 0; i < arraylist_size(scope->function_templates); i++) {
// 		FunctionTemplate *ft = arraylist_get(scope->function_templates, i);

// 		// check if function name matches
// 		if (strcmp(expression->function_identifier, ft->identifier) != 0) {
// 			continue;
// 		} 

// 		// check if number of arguments matches
// 		if (arraylist_size(expression->argument_expression_list->expressions) != arraylist_size(ft->parameter_types)) {
// 			continue;
// 		}

// 		// loop through all arguments
// 		for (size_t j = 0; j < arraylist_size(expression->argument_expression_list->expressions); j++) {
// 			DatatypeOLD *argument_datatype = arraylist_get(arg_types, j);
// 			DatatypeOLD *param_datatype = arraylist_get(ft->parameter_types, j);

// 			// check if argument datatype matches
// 			if (argument_datatype == NULL || param_datatype == NULL) {
// 				return NULL;	
// 			}

// 			if (datatype_is_number(argument_datatype) && datatype_is_number(param_datatype)) {
// 				if (argument_datatype->size > param_datatype->size) {
// 					goto next_function;
// 				}
// 				continue;
// 			} else if (types_equal(argument_datatype, param_datatype)) {
// 				continue;
// 			} else {
// 				goto next_function;
// 			}
// 		}

// 		expression->argument_datatypes = arg_types;

// 		expression->id = ft->id;

// 		return ft->return_typeOLD;

// 		next_function: ;
// 	}

// 	// free arg_types
// 	for (size_t i = 0; i < arraylist_size(arg_types); i++) {
// 		DatatypeOLD *dt = arraylist_get(arg_types, i);
// 		datatype_free(dt);
// 	}
// 	arraylist_free(arg_types);

// 	// no matching function was found
// 	// throw error
// 	printf("    " IRED "error: " RESET "no matching function '%s' found\n", expression->function_identifier);

// 	return NULL;
// }

// DatatypeOLD *type_of_assignment_expression(AssignmentExpression_T *expression, Scope *scope) {
// 	DatatypeOLD *dt_left = type_of_expression(expression->identifier, scope);
// 	DatatypeOLD *dt_right = type_of_expression(expression->assignment_value, scope);

// 	if (dt_left == NULL || dt_right == NULL) {
// 		return NULL;
// 	}

// 	switch(expression->operator) {
// 		case ASSIGNMENT_OPERATOR_DEFAULT: {
// 			if (datatype_is_number(dt_left) && datatype_is_number(dt_right)) {
// 				// check size of numbers
// 				if (dt_left->size < dt_right->size) {
// 					printf("    " IRED "error: " RESET "cannot assign '%s' to '%s'\n",
// 							dt_right->type_identifier,
// 							dt_left->type_identifier);
// 					return NULL;
// 				}
// 			} else if (!types_equal(dt_left, dt_right)) {
// 				printf("    " IRED "error: " RESET "cannot assign '%s' to '%s'\n",
// 						dt_right->type_identifier,
// 						dt_left->type_identifier);
// 				return NULL;
// 			}
// 			return dt_left;
// 		}

// 		case ASSIGNMENT_OPERATOR_ADD:
// 		case ASSIGNMENT_OPERATOR_MULTIPLY:
// 		case ASSIGNMENT_OPERATOR_SUBTRACT:
// 		case ASSIGNMENT_OPERATOR_DIVIDE: {
// 			if (datatype_is_number(dt_left) && datatype_is_number(dt_right)) {
// 				if (dt_left->size < dt_right->size) {
// 					printf("    " IRED "error: " RESET "cannot assign '%s' to '%s'\n",
// 							dt_right->type_identifier,
// 							dt_left->type_identifier);
// 					return NULL;
// 				}
// 			} else {
// 				printf("    " IRED "error: " RESET "assignment operator '%s' cannot be applied to types '%s' and '%s'\n",
// 						"!#[add operator to string function]", dt_left->type_identifier, dt_right->type_identifier);
// 				return NULL;
// 			}
// 			return dt_left;
// 		}
// 	}

// 	return dt_left;
// }

// DatatypeOLD *type_of_array_access_expression(ArrayAccessExpression_T *expression, Scope *scope) {
// 	DatatypeOLD *ident_dt = type_of_literal_expression(expression->identifier, scope);
// 	DatatypeOLD *index_dt = type_of_expression(expression->index_expression, scope);

// 	if (ident_dt == NULL || index_dt == NULL) {
// 		return NULL;
// 	}

// 	if (!datatype_is_number(index_dt)) {
// 		printf("    " IRED "error: " RESET "index must be a number\n");
// 		return NULL;
// 	}

// 	if (strcmp(ident_dt->type_identifier, "string") == 0) {
// 		return parse_datatype("char");
// 	}

// 	return parse_datatype(ident_dt->type_identifier);
// }

// DatatypeOLD *type_of_member_access_expression(MemberAccessExpression_T *expression, Scope *scope) {
// 	// check if expression->identifier is a variable
// 	// if not, check if it is a enum or a struct


// 	if (scope_contains_variable(scope, expression->identifier)) {
// 		VariableTemplate *vt = scope_get_variable_by_name(scope, expression->identifier);

// 		return vt->datatype;
// 	} else if (scope_contains_enum(scope, expression->identifier)) {
// 		EnumDefinition *enum_definition = scope_get_enum_by_name(scope, expression->identifier);

// 		// check if the enum contains a member with the name of the member_identifier of expression
// 		for (size_t i = 0; i < arraylist_size(enum_definition->members); i++) {
// 			EnumDefinitionMember *member = arraylist_get(enum_definition->members, i);
// 			if (strcmp(member->name, expression->member_identifier) == 0) {
// 				DatatypeOLD *dt = parse_datatype(enum_definition->name);
// 				dt->type = DATATYPE_TYPE_ENUM_MEMBER;
// 				return dt;
// 			}
// 		}

// 		printf("    " IRED "error: " RESET "enum '%s' does not contain a member named '%s'\n",
// 				expression->identifier, expression->member_identifier);
// 		return NULL;
// 	} else {
// 		printf("    " IRED "error: " RESET "type or variable '%s' not found\n", expression->identifier);
// 		return NULL;
// 	}

// 	return NULL; // TODO(lucalewin): implement
// }

// // FIXME: this implementation does not look good
// DatatypeOLD *type_of_expression_list(ExpressionList_T *expression_list, Scope *scope) {
// 	DatatypeOLD *datatype = NULL;

// 	for (size_t i = 0; i < arraylist_size(expression_list->expressions); i++) {
// 		Expression_T *expression = arraylist_get(expression_list->expressions, i);
// 		DatatypeOLD *dt = type_of_expression(expression, scope);
// 		if (dt == NULL) {
// 			return NULL;
// 		}
// 		if (datatype == NULL) {
// 			datatype = dt;
// 		} else {

// 			if (datatype_is_number(datatype) && datatype_is_number(dt)) {
// 				if (datatype->size < dt->size) {
// 					datatype = dt;
// 				}
// 			} else if (!types_equal(datatype, dt)) {
// 				printf("    " IRED "error: " RESET "expression list can only contain expressions with the same type, but got '%s' and '%s' instead\n",
// 						dt->type_identifier,
// 						datatype->type_identifier);
// 				return NULL;
// 			}

// 		}
// 	}

// 	return datatype;
// }

// bool variable_types_match(Variable *variable, Scope *scope) {
// 	if (variable->typeOLD == NULL) {
// 		printf("    " IRED "error: " RESET "global variable '%s' (%s:%d:%d) has no type\n", 
// 				variable->identifierOLD->value,
// 				"TODO", 0, 0);

// 		return false;
// 	}

// 	if (variable->initializer != NULL) {
// 		DatatypeOLD *expression_type = type_of_expression(variable->initializer, scope);

// 		if (expression_type == NULL) {
// 			// error will be printed in type_of_expression
// 			return false;
// 		}

// 		// there is an exception for numbers, because the come in different sizes
// 		if (datatype_is_number(expression_type) && datatype_is_number(variable->typeOLD)) {
// 			if (variable->typeOLD->size < expression_type->size) {
// 				printf("    " IRED "error: " RESET "global variable '%s' (%s:%d:%d) has a default value of type '%s' which is smaller than its type '%s'\n", 
// 						variable->identifierOLD->value,
// 						"TODO", 0, 0,
// 						expression_type->type_identifier,
// 						variable->typeOLD->type_identifier);

// 				return false;
// 			}
// 			return true;
// 		}

// 		if (!types_equal(variable->typeOLD, expression_type)) {
// 			printf("    " IRED "error: " RESET "global variable '%s' (%s:%d:%d) has type '%s' but initial value has type '%s'\n", 
// 					variable->identifierOLD->value,
// 					"TODO", 0, 0,
// 					variable->typeOLD->type_identifier,
// 					expression_type->type_identifier);

// 			return false;
// 		}
// 	}

// 	if (variable->typeOLD->is_primitive == 0 && (signed int) variable->typeOLD->size == -1) {
		
// 		if (variable->typeOLD->type == DATATYPE_TYPE_UNKNOWN) {
// 			// check which type it is
// 			if (scope_contains_enum(scope, variable->typeOLD->type_identifier)) {
// 				variable->typeOLD->type = DATATYPE_TYPE_ENUM;
// 			} else {
// 				printf("    " IRED "error: " RESET "global variable '%s' (%s:%d:%d) has type '%s' which is not a struct or enum\n", 
// 						variable->identifierOLD->value,
// 						"TODO", 0, 0,
// 						variable->typeOLD->type_identifier);

// 				return false;
// 			}
// 		}

// 		if (variable->typeOLD->type == DATATYPE_TYPE_STRUCT) {
// 			// check if the struct is defined + set size
// 		}

// 		if (variable->typeOLD->type == DATATYPE_TYPE_ENUM) {
// 			// check if the enum is defined + set size
// 			variable->typeOLD->size = ENUM_MEMBER_SIZE_BYTES;
// 		}

// 		// VariableTemplate *variable_template = scope_get_variable_by_name(scope, variable->identifier->value);
// 		// variable_template->datatype = variable->type;
// 	}

// 	return true;
// }

// bool statement_types_match(Statement *statement, Function *function) {
// 	bool match = true;

// 	switch (statement->type) {
// 		case STATEMENT_COMPOUND: {
// 			CompoundStatement *compound = statement->stmt.compound_statement;

// 			for (size_t i = 0; i < arraylist_size(compound->nested_statements); i++) {
// 				Statement *inner_statement = arraylist_get(compound->nested_statements, i);

// 				if (!statement_types_match(inner_statement, function)) {
// 					match = false;
// 				}
// 			}
// 			break;
// 		}
// 		case STATEMENT_EXPRESSION: {
// 			ExpressionStatement *expression = statement->stmt.expression_statement;

// 			// if all type match will be check inside the function
// 			DatatypeOLD *dt = type_of_expression(expression->expression, statement->scope);
// 			if (dt == NULL) {
// 				match = false;
// 			}

// 			break;
// 		}
// 		case STATEMENT_RETURN: {
// 			ReturnStatement *return_statement = statement->stmt.return_statement;

// 			// check if return type matches
// 			if (return_statement->expression != NULL) {
// 				// evaluate datatype of expression
// 				DatatypeOLD *expression_type = type_of_expression(return_statement->expression, function->scope);

// 				if (expression_type == NULL) {
// 					match = false;
// 					break;
// 				}

// 				// then check if it matches the function's return type
// 				if (datatype_is_number(expression_type) && datatype_is_number(function->return_typeOLD)) {
// 					if (expression_type->size > function->return_typeOLD->size) {
// 						printf("    " IRED "error: " RESET "return statement has type '%s' but function '%s' returns type '%s'\n", 
// 								expression_type->type_identifier,
// 								function->identifier,
// 								function->return_typeOLD->type_identifier);

// 						match = false;
// 					}
// 				} else if (!types_equal(expression_type, function->return_typeOLD)) {
// 					printf("    " IRED "error: " RESET "return statement has type '%s' but function '%s' returns type '%s'\n", 
// 							expression_type->type_identifier,
// 							function->identifier,
// 							function->return_typeOLD->type_identifier);

// 					match = false;
// 				}
// 			} else {
// 				// check if function return type is void, else error
// 				if (strcmp(function->return_typeOLD->type_identifier, "void") != 0) {
// 					printf("    " IRED "error: " RESET "return statement has no value but function '%s' returns type '%s'\n", 
// 							function->identifier,
// 							function->return_typeOLD->type_identifier);

// 					match = false;
// 				}
// 			}
// 			break;
// 		}
// 		case STATEMENT_VARIABLE_DECLARATION: {
// 			VariableDeclarationStatement *variable_declaration = statement->stmt.variable_decl;

// 			if (!variable_types_match(variable_declaration->variable, statement->scope)) {
// 				match = false;
// 			}
// 			break;
// 		}
// 		case STATEMENT_CONDITIONAL: {
// 			ConditionalStatement *conditional = statement->stmt.conditional_statement;

// 			DatatypeOLD *dt_condition = type_of_expression(conditional->condition, statement->scope);

// 			if (strcmp(dt_condition->type_identifier, "bool") != 0) {
// 				printf("    " IRED "error: " RESET "conditional statement has condition of type '%s' but it should be of type 'bool'\n", 
// 						dt_condition->type_identifier);

// 				match = false;
// 			}

// 			if (!statement_types_match(conditional->true_branch, function)) {
// 				match = false;
// 			}

// 			if (conditional->false_branch != NULL) {
// 				if (!statement_types_match(conditional->false_branch, function)) {
// 					match = false;
// 				}
// 			}
// 			break;
// 		}
// 		case STATEMENT_LOOP: {
// 			LoopStatement *loop = statement->stmt.loop_statement;

// 			DatatypeOLD *dt_condition = type_of_expression(loop->condition, statement->scope);

// 			if (strcmp(dt_condition->type_identifier, "bool") != 0) {
// 				printf("    " IRED "error: " RESET "loop statement has condition of type '%s' but it should be of type 'bool'\n", 
// 						dt_condition->type_identifier);

// 				match = false;
// 			}

// 			if (!statement_types_match(loop->body, function)) {
// 				match = false;
// 			}
// 			break;
// 		}
// 		case STATEMENT_ASSEMBLY_CODE_BLOCK: {
// 			// TODO(lucalewin): first implement assembly parsing and
// 			// then check if types match
// 			break;
// 		}
// 	}
// 	return match;
// }

// bool check_if_branch_retuns(ConditionalStatement *conditional_stmt) {
// 	bool true_branch_retuns = false, false_branch_retuns = false;

// 	// check if true branch returns
// 	if (conditional_stmt->true_branch->type == STATEMENT_RETURN) {
// 		true_branch_retuns = true;
// 	} else if (conditional_stmt->true_branch->type == STATEMENT_CONDITIONAL) {
// 		true_branch_retuns = check_if_branch_retuns(conditional_stmt->true_branch->stmt.conditional_statement);
// 	} else if (conditional_stmt->true_branch->type == STATEMENT_COMPOUND) {
// 		CompoundStatement *compound_statement = conditional_stmt->true_branch->stmt.compound_statement;

// 		for (int i = 0; i < arraylist_size(compound_statement->nested_statements); i++) {
// 			Statement *statement = arraylist_get(compound_statement->nested_statements, i);

// 			// check if statement is a branch
// 			if (statement->type == STATEMENT_CONDITIONAL) {
// 				// check if branch returns
// 				ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 				if (check_if_branch_retuns(conditional_statement)) {
// 					true_branch_retuns = true;
// 					break;
// 				}
// 			}

// 			// check if statement is a return statement
// 			if (statement->type == STATEMENT_RETURN) {
// 				true_branch_retuns = true;
// 				break;
// 			}
// 		}
// 	} // else: statement is not a return statement

// 	// check if false branch exists
// 	if (conditional_stmt->false_branch != NULL) {
// 		// check if false branch returns
// 		if (conditional_stmt->false_branch->type == STATEMENT_RETURN) {
// 			false_branch_retuns = true;
// 		} else if (conditional_stmt->false_branch->type == STATEMENT_CONDITIONAL) {
// 			false_branch_retuns = check_if_branch_retuns(conditional_stmt->false_branch->stmt.conditional_statement);
// 		} else if (conditional_stmt->false_branch->type == STATEMENT_COMPOUND) {
// 			CompoundStatement *compound_statement = conditional_stmt->false_branch->stmt.compound_statement;

// 			for (int i = 0; i < arraylist_size(compound_statement->nested_statements); i++) {
// 				Statement *statement = arraylist_get(compound_statement->nested_statements, i);

// 				// check if statement is a branch
// 				if (statement->type == STATEMENT_CONDITIONAL) {
// 					// check if branch returns
// 					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 					if (check_if_branch_retuns(conditional_statement)) {
// 						false_branch_retuns = true;
// 						break;
// 					}
// 				}

// 				// check if statement is a return statement
// 				if (statement->type == STATEMENT_RETURN) {
// 					false_branch_retuns = true;
// 					break;
// 				}
// 			}
// 		} // else: statement is not a return statement
// 	}

// 	return true_branch_retuns && false_branch_retuns;
// }

// bool branch_contains_unreachable_code(ConditionalStatement *conditional_stmt) {

// 	bool true_branch_contains_unreachable_code = false, false_branch_contains_unreachable_code = false;

// 	// check if true branch contains unreachable code
// 	if (conditional_stmt->true_branch->type == STATEMENT_CONDITIONAL) {
// 		ConditionalStatement *true_branch_conditional_stmt = conditional_stmt->true_branch->stmt.conditional_statement;

// 		if (branch_contains_unreachable_code(true_branch_conditional_stmt)) {
// 			true_branch_contains_unreachable_code = true;
// 		}
// 	} else if (conditional_stmt->true_branch->type == STATEMENT_COMPOUND) {
// 		CompoundStatement *compound_statement = conditional_stmt->true_branch->stmt.compound_statement;

// 		for (size_t k = 0; k < arraylist_size(compound_statement->nested_statements); k++) {
// 			Statement *statement = arraylist_get(compound_statement->nested_statements, k);

// 			if (statement->type == STATEMENT_CONDITIONAL) {
// 				ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 				if (branch_contains_unreachable_code(conditional_statement)) {
// 					true_branch_contains_unreachable_code = true;
// 				}

// 				if (check_if_branch_retuns(conditional_statement)) {
// 					// check if more statements exist after the branch
// 					if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
// 						// there are statements after the conditional statement
// 						// which cannot be reached because every branch of the
// 						// conditional statement returns
// 						printf("%s:%d:%d: " YELLOW "warning: " RESET "function '%s' contains unreachable code\n", 
// 								//function->identifier,
// 								"TODO", 0, 0,
// 								"TODO");
						
// 						true_branch_contains_unreachable_code = true;

// 						for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
// 						{
// 							Statement *statement = arraylist_get(compound_statement->nested_statements, l);
// 							statement_free(statement);
// 							arraylist_remove_at_index(compound_statement->nested_statements, l);
// 						}
// 					}
// 				}
// 			} else if (statement->type == STATEMENT_RETURN) {
// 				// check if more statements exist after the return statement
// 				if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
// 					// there are statements after the return statement
// 					// which cannot be reached because the function returns
// 					printf("%s:%d:%d: " YELLOW "warning: " RESET "function '%s' contains unreachable code\n", 
// 							//function->identifier,
// 							"TODO", 0, 0,
// 							"TODO");
					
// 					true_branch_contains_unreachable_code = true;

// 					for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
// 					{
// 						Statement *statement = arraylist_get(compound_statement->nested_statements, l);
// 						statement_free(statement);
// 						arraylist_remove_at_index(compound_statement->nested_statements, l);
// 					}
// 				}
// 			} else if (statement->type == STATEMENT_LOOP) {
// 				// TODO(lucalewin): check if loop condition is always true
// 				// if it is, then check if the loop breaks
// 				// else the code after 
// 			}
// 		}
// 	} else if (conditional_stmt->true_branch->type == STATEMENT_LOOP) {
// 		// TODO(lucalewin): check if loop condition is always true
// 		// if it is, then check if the loop breaks
// 		// else the code after 
// 	} // else: statement is not a branch

// 	// check if false branch contains unreachable code
// 	// check if false branch exists
// 	if (conditional_stmt->false_branch != NULL) {
// 		// check if false branch returns
// 		if (conditional_stmt->false_branch->type == STATEMENT_RETURN) {
// 			false_branch_contains_unreachable_code = true;
// 		} else if (conditional_stmt->false_branch->type == STATEMENT_CONDITIONAL) {
// 			ConditionalStatement *false_branch_conditional_stmt = conditional_stmt->false_branch->stmt.conditional_statement;

// 			if (branch_contains_unreachable_code(false_branch_conditional_stmt)) {
// 				false_branch_contains_unreachable_code = true;
// 			}
// 		} else if (conditional_stmt->false_branch->type == STATEMENT_COMPOUND) {
// 			CompoundStatement *compound_statement = conditional_stmt->false_branch->stmt.compound_statement;

// 			for (size_t k = 0; k < arraylist_size(compound_statement->nested_statements); k++) {
// 				Statement *statement = arraylist_get(compound_statement->nested_statements, k);

// 				if (statement->type == STATEMENT_CONDITIONAL) {
// 					ConditionalStatement *conditional_statement = statement->stmt.conditional_statement;

// 					if (branch_contains_unreachable_code(conditional_statement)) {
// 						false_branch_contains_unreachable_code = true;
// 					}

// 					if (check_if_branch_retuns(conditional_statement)) {
// 						// check if more statements exist after the branch
// 						if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
// 							// there are statements after the conditional statement
// 							// which cannot be reached because every branch of the
// 							// conditional statement returns
// 							printf("%s:%d:%d: " YELLOW "warning: " RESET "function '%s' contains unreachable code\n", 
// 									//function->identifier,
// 									"TODO", 0, 0,
// 									"TODO");
							
// 							false_branch_contains_unreachable_code = true;

// 							for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
// 							{
// 								Statement *statement = arraylist_get(compound_statement->nested_statements, l);
// 								statement_free(statement);
// 								arraylist_remove_at_index(compound_statement->nested_statements, l);
// 							}
// 						}
// 					}
// 				} else if (statement->type == STATEMENT_RETURN) {
// 					// check if more statements exist after the return statement
// 					if (k + 1 < arraylist_size(compound_statement->nested_statements)) {
// 						// there are statements after the return statement
// 						// which cannot be reached because the function returns
// 						printf("%s:%d:%d: " YELLOW "warning: " RESET "function '%s' contains unreachable code\n", 
// 								//function->identifier,
// 								"TODO", 0, 0,
// 								"TODO");
						
// 						false_branch_contains_unreachable_code = true;

// 						for (size_t l = arraylist_size(compound_statement->nested_statements) - 1; l > k; l--)
// 						{
// 							Statement *statement = arraylist_get(compound_statement->nested_statements, l);
// 							statement_free(statement);
// 							arraylist_remove_at_index(compound_statement->nested_statements, l);
// 						}
// 					}
// 				} else if (statement->type == STATEMENT_LOOP) {
// 					// TODO(lucalewin): check if loop condition is always true
// 					// if it is, then check if the loop breaks
// 					// else the code after 
// 				}
// 			}
// 		} else if (conditional_stmt->false_branch->type == STATEMENT_LOOP) {
// 			// TODO(lucalewin): check if loop condition is always true
// 			// if it is, then check if the loop breaks
// 			// else the code after 
// 		} // else: statement is not a branch
// 	}

// 	return true_branch_contains_unreachable_code || false_branch_contains_unreachable_code;
// }
