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
 * @brief free a literal
 * 
 * @param literal the literal to free
 */
void literal_free(Literal *literal) {
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
			for (size_t i = 0; i < statement->stmt.compound->nested_statements->size; i++) {
				Statement *stmt = arraylist_get(statement->stmt.compound->nested_statements, i);
				statement_free(stmt);
			}
			scope_free(statement->stmt.compound->local_scope);
			break;
		case STATEMENT_EXPRESSION:
			expression_free(statement->stmt.expression->expression);
			free(statement->stmt.expression);
			break;
		case STATEMENT_RETURN:
			expression_free(statement->stmt._return->expression);
			free(statement->stmt._return);
			break;
		case STATEMENT_VARIABLE_DECLARATION:
			variable_free(statement->stmt.variable_declaration->variable);
			free(statement->stmt.variable_declaration);
			break;
		case STATEMENT_CONDITIONAL:
			expression_free(statement->stmt.conditional->condition);
			statement_free(statement->stmt.conditional->true_branch);
			if (statement->stmt.conditional->false_branch != NULL) {
				statement_free(statement->stmt.conditional->false_branch);
			}
			break;
		case STATEMENT_LOOP:
			expression_free(statement->stmt.loop->condition);
			statement_free(statement->stmt.loop->body);
			break;
		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			free(statement->stmt.assembly->code);
			free(statement->stmt.assembly);
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
