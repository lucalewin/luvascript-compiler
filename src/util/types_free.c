#include <parsing/nodes/ast.h>
#include <parsing/nodes/literal.h>
#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/import.h>
#include <options.h>

#include <parsing/scope_impl.h>
#include <util/arraylist.h>

#include <util/logging/logger.h>

/**
 * @brief free an expression
 * 
 * @param expression the expression to free
 */
void expression_free(Expression_T *expression) {
	if (expression == NULL) {
		return;
	}

	switch (expression->type) {
		case EXPRESSION_TYPE_LITERAL:
			literal_free(expression->expr.literal_expr);
			break;
		case EXPRESSION_TYPE_UNARY:
			literal_free(expression->expr.unary_expr->identifier);
			break;
		case EXPRESSION_TYPE_BINARY:
			expression_free(expression->expr.binary_expr->expression_left);
			expression_free(expression->expr.binary_expr->expression_right);
			break;
		case EXPRESSION_TYPE_NESTED:
			expression_free(expression->expr.nested_expr->expression);
			break;
		case EXPRESSION_TYPE_FUNCTION_CALL:
			free(expression->expr.func_call_expr->function_identifier);
			for (size_t i = 0; i < expression->expr.func_call_expr->argument_expression_list->expressions->size; i++) {
				Expression_T *argument = arraylist_get(expression->expr.func_call_expr->argument_expression_list->expressions, i);
				expression_free(argument);
			}
			break;
		case EXPRESSION_TYPE_ASSIGNMENT:
			expression_free(expression->expr.assignment_expr->identifier);
			expression_free(expression->expr.assignment_expr->assignment_value);
			break;
		case EXPRESSION_TYPE_ARRAY_ACCESS:
			literal_free(expression->expr.array_access_expr->identifier);
			expression_free(expression->expr.array_access_expr->index_expression);
			break;
		case EXPRESSION_TYPE_MEMBER_ACCESS:
			expression_free(expression->expr.member_access_expr->identifier_old);
			free(expression->expr.member_access_expr->member_identifier);
			break;
		case EXPRESSION_TYPE_LIST:
			for (size_t i = 0; i < expression->expr.list_expr->expressions->size; i++) {
				Expression_T *expr = arraylist_get(expression->expr.list_expr->expressions, i);
				expression_free(expr);
			}
			arraylist_free(expression->expr.list_expr->expressions);
			free(expression->expr.list_expr);
			break;
	}

	free(expression);
}

/**
 * @brief free a literal
 * 
 * @param literal the literal to free
 */
void literal_free(Literal_T *literal) {
	free(literal->value);
	free(literal);
}

/**
 * @brief free a statement
 * 
 * @param statement the statement to free
 */
void statement_free(Statement *statement) {
	scope_free(statement->scope);

	switch (statement->type) {
		case STATEMENT_COMPOUND:
			for (size_t i = 0; i < statement->stmt.compound_statement->nested_statements->size; i++) {
				Statement *stmt = arraylist_get(statement->stmt.compound_statement->nested_statements, i);
				statement_free(stmt);
			}
			scope_free(statement->stmt.compound_statement->local_scope);
			break;
		case STATEMENT_EXPRESSION:
			expression_free(statement->stmt.expression_statement->expression);
			free(statement->stmt.expression_statement);
			break;
		case STATEMENT_RETURN:
			expression_free(statement->stmt.return_statement->expression);
			free(statement->stmt.return_statement);
			break;
		case STATEMENT_VARIABLE_DECLARATION:
			variable_free(statement->stmt.variable_decl->variable);
			free(statement->stmt.variable_decl);
			break;
		case STATEMENT_CONDITIONAL:
			expression_free(statement->stmt.conditional_statement->condition);
			statement_free(statement->stmt.conditional_statement->true_branch);
			if (statement->stmt.conditional_statement->false_branch != NULL) {
				statement_free(statement->stmt.conditional_statement->false_branch);
			}
			break;
		case STATEMENT_LOOP:
			expression_free(statement->stmt.loop_statement->condition);
			statement_free(statement->stmt.loop_statement->body);
			break;
		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			free(statement->stmt.assembly_code_block_statement->code);
			free(statement->stmt.assembly_code_block_statement);
			break;
	}

	free(statement);
}

// void package_free(Package *package) {
// 	if (package == NULL) {
// 		return;
// 	}
// 	free(package->name);
// 	free(package->file_path);
// 	scope_free(package->package_scope);

// 	for (size_t i = 0; i < package->import_declarations->size; i++) {
// 		ImportDeclaration *import_declaration = arraylist_get(package->import_declarations, i);
// 		import_declaration_free(import_declaration);
// 	}

// 	if (package->functions != NULL) {
// 		for (size_t i = 0; i < package->functions->size; i++) {
// 			Function *function = arraylist_get(package->functions, i);
// 			function_free(function);
// 		}
// 		arraylist_free(package->functions);
// 	}

// 	for (size_t i = 0; i < package->extern_functions->size; i++) {
// 		FunctionTemplate *function_template = arraylist_get(package->extern_functions, i);
// 		function_template_free(function_template);
// 	}

// 	for (size_t i = 0; i < package->global_variables->size; i++) {
// 		Variable *variable = arraylist_get(package->global_variables, i);
// 		variable_free(variable);
// 	}

// 	// free imported functions and variables
// 	for (size_t i = 0; i < package->imported_functions->size; i++) {
// 		FunctionTemplate *function = arraylist_get(package->imported_functions, i);
// 		function_template_free(function);
// 	}

// 	for (size_t i = 0; i < package->imported_global_variables->size; i++) {
// 		VariableTemplate *variable = arraylist_get(package->imported_global_variables, i);
// 		variable_template_free(variable);
// 	}
// }

void options_free(CommandlineOptions *options) {
	if (options == NULL) return;
	free(options->input_file_name);
	free(options->output_file_name);
	arraylist_free(options->library_paths);
	free(options);
}
