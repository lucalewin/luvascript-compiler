#include <types/ast.h>
#include <types/literal.h>
#include <types/datatypes.h>
#include <types/expression.h>
#include <types/function.h>
#include <types/statement.h>
#include <types/variable.h>
#include <types/package.h>
#include <types/import.h>
#include <options.h>

#include <scope_impl.h>
#include <util/arraylist.h>

#include <logging/logger.h>

/**
 * @brief free a ast
 * 
 * @param ast the ast to free
 */


/**
 * @brief free a datatype
 * 
 * @param datatype the datatype to free
 */
void datatype_free(Datatype *datatype) {
	if (datatype == NULL) return;
	if (!datatype->is_primitive)
		free(datatype->type_identifier);
	free(datatype);
}

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
		case EXPRESSION_TYPE_FUNCTIONCALL:
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
		case EXPRESSION_TYPE_ARRAYACCESS:
			literal_free(expression->expr.array_access_expr->identifier);
			expression_free(expression->expr.array_access_expr->index_expression);
			break;
		case EXPRESSION_TYPE_MEMBERACCESS:
			expression_free(expression->expr.member_access_expr->identifier);
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
 * @brief free a function
 * 
 * @param function the function to free
 */
void function_free(Function *function) {
	free(function->identifier);

	for (size_t i = 0; i < function->parameters->size; i++) {
		Variable *variable = arraylist_get(function->parameters, i);
		variable_free(variable);
	}

	datatype_free(function->return_type);

	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *statement = arraylist_get(function->body_statements, i);
		statement_free(statement);
	}

	scope_free(function->scope);
}

/**
 * @brief free a function template
 * 
 * @param function_template the function template to free
 */
void function_template_free(FunctionTemplate *function_template) {
	free(function_template->identifier);

	for (size_t i = 0; i < function_template->param_datatypes->size; i++) {
		Datatype *datatype = arraylist_get(function_template->param_datatypes, i);
		datatype_free(datatype);
	}
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

/**
 * @brief free a variable
 * 
 * @param variable the variable to free
 */
void variable_free(Variable *variable) {
	if (variable == NULL) {
		return;
	}
	datatype_free(variable->datatype);
	literal_free(variable->identifier);
	expression_free(variable->default_value);
	free(variable);
}

void package_free(Package *package) {
	free(package->name);
	scope_free(package->package_scope);

	// for (size_t i = 0; i < package->imported_packages->size; i++) {
	// 	Package *imported_package = arraylist_get(package->imported_packages, i);
	// 	package_free(imported_package);
	// }

	for (size_t i = 0; i < package->functions->size; i++) {
		Function *function = arraylist_get(package->functions, i);
		function_free(function);
	}

	for (size_t i = 0; i < package->global_variables->size; i++) {
		FunctionTemplate *function_template = arraylist_get(package->global_variables, i);
		function_template_free(function_template);
	}

	for (size_t i = 0; i < package->extern_functions->size; i++) {
		Variable *variable = arraylist_get(package->extern_functions, i);
		variable_free(variable);
	}
}

void options_free(CommandlineOptions *options) {
	free(options->input_file_name);
	free(options->output_file_name);
	arraylist_free(options->library_paths);
	free(options);
}

void import_stmt_free(import_stmt_t *import_stmt) {
	free(import_stmt->package_name);
	free(import_stmt->type_identifier);
	free(import_stmt);
}