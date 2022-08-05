#include <compiler/arch/x86-64/compiler.h>

// standart libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility libraries
#include <util/util.h>
#include <util/io/cmd.h>
#include <util/logging/logger.h>

#include <parsing/scope_impl.h>
#include <generation/arch/x86-64/assembly/datatypes.h>
#include <generation/arch/x86-64/assembly/registers.h>

// nodes
#include <parsing/nodes/operator.h>
#include <parsing/nodes/literal.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/datatypes.h>

#include <compiler/arch/x86-64/optimizer.h>

// ------------------------ string templates ------------------------

char *header_template =
"section .text\n\n"
"global _start\n"
"_start:\n";

char *exit_template = 
"\tmov rax, 60\n"
"\tsyscall\n\n";

// ------------------------- global variables --------------------------

int label_counter = 0;
int statement_label_counter = 0;

// ----------------------- function prototypes -------------------------

char *compile_imports(Package *package);
char *compile_global_variable(Variable *glob_var);
char *compile_extern_function_templates(FunctionTemplate *func_template);
char *compile_function(Function *function);

char *compile_statement(Statement *stmt);
char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope);
char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope);
char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope);
char *compile_expression_statement(ExpressionStatement *expr_stmt, Scope *scope);
char *compile_conditional_statement(ConditionalStatement *cond_stmt, Scope *scope);
char *compile_loop_statement(LoopStatement *loop_stmt, Scope *scope);

char *compile_expression(Expression_T *expr, Scope *scope);
char *compile_literal_expression(Literal_T *literal, Scope *scope);
char *compile_unary_expression(UnaryExpression_T *unary_expr, Scope *scope);
char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope);
char *compile_nested_expression(NestedExpression_T *nested_expr, Scope *scope);
char *compile_function_call_expression(FunctionCallExpression_T *func_call_expr, Scope *scope);
char *compile_assignment_expression(AssignmentExpression_T *assignment_expr, Scope *scope);
char *compile_array_access_expression(ArrayAccessExpression_T *array_access_expr, Scope *scope);
char *compile_member_access_expression(MemberAccessExpression_T *member_access_expr, Scope *scope);
char *compile_list_expression(ExpressionList_T *list_expr, Scope *scope);

char *compile_variable_pointer(VariableTemplate *var_template, Scope *scope);
char *compile_variable_pointer_with_offset(VariableTemplate *var_template, Scope *scope);

char *dereference_pointer_variable(VariableTemplate *var_template, Scope *scope);
char *reference_pointer_variable(VariableTemplate *var_template, Scope *scope);

// --------------------- function implementations ----------------------

char *compile_to_x86_64_assembly(AST *ast, CommandlineOptions *options) {
	log_info("compiling to x68_64 assembly\n");

	char *asm_code = calloc(1, sizeof(char));

	for (size_t i = 0; i < ast->packages->size; i++) {
		Package *package = arraylist_get(ast->packages, i);

		asm_code = straddall(asm_code, 
				";\n"
				"; package '", package->name, "'\n"
				";\n", NULL);

		if (package->global_variables->size > 0) {
			asm_code = stradd(asm_code, "section .data\n");

			for (size_t i = 0; i < package->global_variables->size; i++) {
				asm_code = stradd(asm_code, compile_global_variable(arraylist_get(package->global_variables, i)));
			}
		}

		asm_code = stradd(asm_code, compile_imports(package));

		// check if main method is defined
		if (scope_contains_function(package->package_scope, "main")) {
			// main function is defined
			FunctionTemplate *main_func_temp = scope_get_function_by_name(package->package_scope, "main");

			char *main_func_lcc_identifier = function_to_lcc_identifier(main_func_temp);

			asm_code = stradd(asm_code, header_template);
			asm_code = stradd(asm_code, "\tcall ");
			asm_code = stradd(asm_code, main_func_lcc_identifier);
			asm_code = stradd(asm_code, "\n\tmov rdi, rax\n");
			asm_code = stradd(asm_code, exit_template);

			free(main_func_lcc_identifier);
		} else {
			asm_code = stradd(asm_code, "section .text\n");
		}

		char *function_code = calloc(1, sizeof(char));
		char *global_function_decl_code = "global ";

		// compile functions
		for (int i = 0; i < package->functions->size; i++) {
			Function *func = arraylist_get(package->functions, i);

			if (i > 0) {
				global_function_decl_code = stradd(global_function_decl_code, ", ");
			}
			global_function_decl_code = stradd(global_function_decl_code, function_to_lcc_identifier(convert_to_function_template(func)));

			function_code = stradd(function_code, compile_function(func));
		}

		global_function_decl_code = stradd(global_function_decl_code, "\n\n");

		// define external functions
		for (size_t i = 0; i < package->extern_functions->size; i++) {
			function_code = stradd(function_code, compile_extern_function_templates(arraylist_get(package->extern_functions, i)));
		}
		
		asm_code = straddall(asm_code, global_function_decl_code, function_code, NULL);
	}


	return asm_code;
}

char *compile_imports(Package *package) {
	char *imports = calloc(1, sizeof(char));

	for (size_t i = 0; i < arraylist_size(package->imported_global_variables); i++) {
		VariableTemplate *var_template = arraylist_get(package->imported_global_variables, i);

		imports = stradd(imports, "extern ");
		imports = stradd(imports, variable_to_lcc_identifier(var_template));
		imports = stradd(imports, "\n");
	}
	for (size_t i = 0; i < arraylist_size(package->imported_functions); i++) {
		FunctionTemplate *func_template = arraylist_get(package->imported_functions, i);

		imports = stradd(imports, "extern ");
		imports = stradd(imports, function_to_lcc_identifier(func_template));
		imports = stradd(imports, "\n");
	}


	return imports;
}

char *compile_global_variable(Variable *glob_var) {

	char *variable_value = NULL;

	switch (glob_var->default_value->type) {
		case EXPRESSION_TYPE_LITERAL: {
			if (glob_var->default_value->expr.literal_expr->type == LITERAL_STRING) {
				// use backquote for escaping the literal string in nasm syntax
				variable_value = "`";
				variable_value = stradd(variable_value, glob_var->default_value->expr.literal_expr->value);
				variable_value = stradd(variable_value, "`");
				variable_value = stradd(variable_value, ",0");
			} else {
				variable_value = glob_var->default_value->expr.literal_expr->value;
			}
			break;
		}
		case EXPRESSION_TYPE_LIST: {
			ExpressionList_T *list_expr = glob_var->default_value->expr.list_expr;

			for (size_t i = 0; i < list_expr->expressions->size; i++) {
				Expression_T *expr = arraylist_get(list_expr->expressions, i);
				if (expr->type != EXPRESSION_TYPE_LITERAL) {
					log_error("default value of global variable must be a literal\n");
					return NULL;
				}

				if (variable_value == NULL) {
					// variable_value = stradd(variable_value, expr->expr.literal_expr->value);
					// copy value into variable_value using calloc and strcpy
					variable_value = calloc(strlen(expr->expr.literal_expr->value), sizeof(char));
					strcpy(variable_value, expr->expr.literal_expr->value);

				} else {
					variable_value = straddall(variable_value, ",", expr->expr.literal_expr->value, NULL);
				}

			}
			straddall(variable_value, "\n", NULL);

			break;
		}
		default:
			log_error("only global variables with literal or list expressions are supported yet\n");
			return NULL;
	}

	char *var_lcc_identifier = variable_to_lcc_identifier(convert_to_variable_template(glob_var));
	char *asm_code = calloc(strlen(var_lcc_identifier) + 2, sizeof(char));
	strcpy(asm_code, var_lcc_identifier);
	strcat(asm_code, ":");


	switch (glob_var->type->size) {
		case 1: // BYTE
			asm_code = straddall(asm_code, " db ", variable_value, "\n", NULL);
			break;
		case 2: // WORD
			asm_code = straddall(asm_code, " dw ", variable_value, "\n", NULL);
			break;
		case 4: // DWORD
			asm_code = straddall(asm_code, " dd ", variable_value, "\n", NULL);
			break;
		case 8: // QWORD
			asm_code = straddall(asm_code, " dq ", variable_value, "\n", NULL);
			break;
		default:
			log_error("unknown variable size\n");
			exit(1);
	}

	asm_code = straddall(asm_code, "global ", var_lcc_identifier, "\n", NULL);

	free(var_lcc_identifier);
	free(variable_value);

	return asm_code;
}

char *compile_extern_function_templates(FunctionTemplate *func_template) {
	// return straddall("extern ", func_template->identifier, "\n", NULL);
	return straddall("extern ", function_to_lcc_identifier(func_template), "\n", NULL);
}

char *compile_function(Function *function) {
	char *function_code = calloc(1, sizeof(char));
	char *func_lcc_ident = function_to_lcc_identifier(convert_to_function_template(function));

	function_code = straddall(function_code,
	// "global ", func_lcc_ident, "\n",
	func_lcc_ident, ":\n", NULL);

	free(func_lcc_ident);

	// align stack

	size_t size_for_stack_align = 0;
	for (int i = 0; i < function->scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(function->scope->local_variable_templates, i);
		// if (var_template->datatype->is_array) {
		// 	size_for_stack_align += var_template->datatype->size * var_template->datatype->array_size;
		// } else
		if (var_template->datatype->is_pointer || var_template->datatype->is_array) {
			size_for_stack_align += 8;
		} else {
			size_for_stack_align += var_template->datatype->size;
		}
	}

	// compile statements
	char *stmts_code = calloc(1, sizeof(char));
	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *stmt = arraylist_get(function->body_statements, i);
		stmts_code = stradd(stmts_code, compile_statement(stmt));
	}

	if (size_for_stack_align != 0) {
		function_code = stradd(function_code, "\tpush rbp\n"); // save old value of RBP
		function_code = stradd(function_code, "\tmov rbp, rsp\n");
		function_code = stradd(function_code, "\tsub rsp, "); // align stack
		function_code = stradd(function_code, int_to_string(size_for_stack_align + 8)); // size in bytes
		function_code = stradd(function_code, "\n");
	}

	// compile args

	switch (function->parameters->size) {
		case 6: {
			Variable *param = arraylist_get(function->parameters, 5);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_R9, template->datatype->is_pointer || template->datatype->is_array ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 5: {
			Variable *param = arraylist_get(function->parameters, 4);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_R8, template->datatype->is_pointer || template->datatype->is_array ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 4: {
			Variable *param = arraylist_get(function->parameters, 3);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_RCX, template->datatype->is_pointer ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 3: {
			Variable *param = arraylist_get(function->parameters, 2);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_RDX, template->datatype->is_pointer || template->datatype->is_array ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 2: {
			Variable *param = arraylist_get(function->parameters, 1);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_RSI, template->datatype->is_pointer || template->datatype->is_array ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 1: {
			Variable *param = arraylist_get(function->parameters, 0);
			VariableTemplate *template = convert_to_variable_template(param);

			char *pointer = compile_variable_pointer(template, function->scope);
			char *reg = register_toString(REGISTER_RDI, template->datatype->is_pointer || template->datatype->is_array ? 8 : template->datatype->size);
			function_code = straddall(function_code, "\tmov ", pointer, ", ", reg, "\t; function paramter: ", template->identifier, "\n", NULL);

			variable_template_free(template);
			free(pointer);
		}
		case 0:
			break;
		default:
			break;
	}

	
	function_code = straddall(function_code, stmts_code, "\n", NULL);

	return function_code;
}

char *compile_statement(Statement *stmt) {
	// log_debug("compile_statement(): statement type = %s\n", STATEMENT_TYPES[stmt->type]);

	switch(stmt->type) {
		case STATEMENT_RETURN:
			return compile_return_statement(stmt->stmt.return_statement, stmt->scope);

		case STATEMENT_COMPOUND:
			return compile_compound_statement(stmt->stmt.compound_statement, stmt->scope);

		case STATEMENT_VARIABLE_DECLARATION:
			return compile_variable_declaration_statement(stmt->stmt.variable_decl, stmt->scope);

		case STATEMENT_EXPRESSION:
			return compile_expression_statement(stmt->stmt.expression_statement, stmt->scope);

		case STATEMENT_CONDITIONAL:
			return compile_conditional_statement(stmt->stmt.conditional_statement, stmt->scope);

		case STATEMENT_LOOP:
			return compile_loop_statement(stmt->stmt.loop_statement, stmt->scope);

		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			return stmt->stmt.assembly_code_block_statement->code;

		default:
			log_error("compile_statement(): unexpected statement type '%s'\n", STATEMENT_TYPES[stmt->type]);
			exit(1);
	}
}

char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope) {
	// align stack if needed (for local variables)
	int size_for_stack_align = 0;
	for (size_t i = 0; i < compound_stmt->local_scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(compound_stmt->local_scope->local_variable_templates, i);
		size_for_stack_align += var_template->datatype->size;
	}

	char *compound_code = calloc(1, sizeof(char));

	if (size_for_stack_align > 0) {
		compound_code = straddall(compound_code, 
				// "push rbp\n"
				// "mov rbp, rsp\n"
				"\tsub rsp, ", int_to_string(size_for_stack_align), "\n", NULL);
	}

	for (size_t i = 0; i < compound_stmt->nested_statements->size; i++) {
		Statement *stmt = arraylist_get(compound_stmt->nested_statements, i);
		compound_code = stradd(compound_code, compile_statement(stmt));
	}

	if (size_for_stack_align != 0) {
		// compound_code = stradd(compound_code, "mov rsp, rbp\npop rbp\n");
		compound_code = straddall(compound_code, "\tadd rsp, ", int_to_string(size_for_stack_align), "\n", NULL);
	}

	return compound_code;
}

char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope) {
	char *ret_stmt_code = ret_stmt->expression == NULL ? "" : compile_expression(ret_stmt->expression, scope);

	// reset base pointer
	size_t size_for_stack_align = 0;
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(scope->local_variable_templates, i);
		size_for_stack_align += var_template->datatype->size;
	}

	if (size_for_stack_align != 0) {
		// ret_stmt_code = stradd(ret_stmt_code, "\tmov rsp, rbp\n\tpop rbp\n");
		ret_stmt_code = stradd(ret_stmt_code, "\tleave\n");
	}

	ret_stmt_code = stradd(ret_stmt_code, "\tret\n");

	return ret_stmt_code;
}

char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope) {
	// log_debug("compile_variable_declaration_statement():\n");

	Variable *var = var_decl_stmt->variable;	

	// compile default expression
	// TODO(lucalewin): simplify default value assignment
	// if (var->default_value->type == EXPRESSION_LITERAL) {}

	char *var_decl_code = compile_expression(var->default_value, scope);

	VariableTemplate *var_template = convert_to_variable_template(var);

	// because the current scope does not contain the new variable
	// scope_get_variable_rbp_offset adds the sizes of all previously
	// declared variables to the offset, after which the new variable
	// is added
	int offset = scope_get_variable_rbp_offset(scope, var_decl_stmt->variable->identifier->value);

	// TODO(lucalewin): simplify this
	switch (var_template->datatype->size) {
		case 1: // BYTE
			var_decl_code = straddall(var_decl_code, "\tmov BYTE[rbp-", int_to_string(offset), "],", NULL);
			break;
		case 2: // WORD
			var_decl_code = straddall(var_decl_code, "\tmov WORD[rbp-", int_to_string(offset), "],", NULL);
			break;
		case 4: // DWORD
			var_decl_code = straddall(var_decl_code, "\tmov DWORD[rbp-", int_to_string(offset), "],", NULL);
			break;
		case 8: // QWORD
			var_decl_code = straddall(var_decl_code, "\tmov QWORD[rbp-", int_to_string(offset), "],", NULL);
			break;
		default:
			log_error("unknown datatype size [#1]: %d\n", var_template->datatype->size);
			exit(1);
	}

	var_decl_code = straddall(var_decl_code,
					register_toString(REGISTER_RAX, var->type->size), 
					"\t; variable: ", var->identifier->value, "\n", NULL);

	return var_decl_code;
}

char *compile_expression_statement(ExpressionStatement *expr_stmt, Scope *scope) {
	char *expr_stmt_code = compile_expression(expr_stmt->expression, scope);

	return expr_stmt_code;
}

char *compile_conditional_statement(ConditionalStatement *cond_stmt, Scope *scope) {
	// compile condition expression
	char *cond_expr_code = compile_expression(cond_stmt->condition, scope);

	// compile true branch
	char *true_branch_code = compile_statement(cond_stmt->true_branch);

	// generate label for false branch (CF = condition false)
	char *label_condition_false = stradd(".CF", int_to_string(label_counter++));

	// if false branch is empty, just return the true branch
	if (cond_stmt->false_branch == NULL) {
		return straddall(
					"; condition\n", 
					cond_expr_code, 
					"\tcmp eax, 0\n"
					"\tje ", label_condition_false, "\n", 
					true_branch_code, 
					label_condition_false, ":\n", NULL);
	}

	// else statement is not empty
	Statement *false_branch_statement = cond_stmt->false_branch;
	
	// compile false branch
	char *false_branch_code = compile_statement(false_branch_statement);

	// generate label for end of conditional statement (CE = condition end)
	char *end_of_conditional_statement = stradd(".CE", int_to_string(statement_label_counter)); 

	return straddall(
				cond_expr_code, 
				"\tcmp eax, 0\n"
				"\tje ", label_condition_false, "\n", 
				true_branch_code, 
				"\tjmp ", end_of_conditional_statement, "\n", 
				label_condition_false, ":\n", 
				false_branch_code,
				end_of_conditional_statement, ":\n", NULL);
}

char *compile_loop_statement(LoopStatement *loop_stmt, Scope *scope) {
	// compile condition expression
	char *cond_expr_code = compile_expression(loop_stmt->condition, scope);
	// compile loop body
	char *loop_body_code = compile_statement(loop_stmt->body);

	// generate label for false condition
	char *label_condition = stradd(".LC", int_to_string(label_counter++));
	char *label_condition_false = stradd(".LCF", int_to_string(label_counter++));

	return straddall(
				label_condition, ":\t; while () {\n", 
				cond_expr_code, 
				"\tcmp eax, 0\n"
				"\tje ", label_condition_false, "\n", 
				loop_body_code, 
				"\tjmp ", label_condition, "\t;}\n",
				label_condition_false ,":\n", NULL);
}

// ----------------------------------------------------------------

char *compile_expression(Expression_T *expr, Scope *scope) {
	
	// try to simplify the expression
	expr = simplify_expression(expr);

	// print_expression(expr);
	// printf("\n\n");

	switch(expr->type) {
		case EXPRESSION_TYPE_LITERAL:
			return compile_literal_expression(expr->expr.literal_expr, scope);

		case EXPRESSION_TYPE_UNARY:
			return compile_unary_expression(expr->expr.unary_expr, scope);

		case EXPRESSION_TYPE_BINARY:
			return compile_binary_expression(expr->expr.binary_expr, scope);

		case EXPRESSION_TYPE_NESTED:
			return compile_nested_expression(expr->expr.nested_expr, scope);

		case EXPRESSION_TYPE_FUNCTIONCALL:
			return compile_function_call_expression(expr->expr.func_call_expr, scope);

		case EXPRESSION_TYPE_ASSIGNMENT:
			return compile_assignment_expression(expr->expr.assignment_expr, scope);

		case EXPRESSION_TYPE_ARRAYACCESS:
			return compile_array_access_expression(expr->expr.array_access_expr, scope);

		case EXPRESSION_TYPE_MEMBERACCESS:
			return compile_member_access_expression(expr->expr.member_access_expr, scope);

		case EXPRESSION_TYPE_LIST:
			return compile_list_expression(expr->expr.list_expr, scope);
	}

	log_error("compile_expression(): returning NULL\n");
	return NULL;
}

char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope) {
	char *bin_expr_code = calloc(1, sizeof(char));

	bin_expr_code = stradd(bin_expr_code, compile_expression(bin_expr->expression_left, scope));

	switch(bin_expr->expression_right->type) {
		case EXPRESSION_TYPE_LITERAL: {
			Literal_T *literal = bin_expr->expression_right->expr.literal_expr;

			switch (literal->type) {
				case LITERAL_BOOLEAN: // a boolean is also just a number (0 or 1)
				case LITERAL_NUMBER:
				case LITERAL_CHARACTER: // a character is also a number
				{
					if (literal->type == LITERAL_CHARACTER) {
						char *char_code = int_to_string((int) *(literal->value));
						free(literal->value);
						literal->value = char_code;
						// log_debug("compile_binary_expression(): char_code = %s\n", char_code);
					}
					
					switch (bin_expr->operator) {
						case BINARY_OPERATOR_ADD:
							bin_expr_code = straddall(bin_expr_code, "\tadd rax, ", literal->value, "\n", NULL);
							break;
						case BINARY_OPERATOR_SUBTRACT:
							bin_expr_code = straddall(bin_expr_code, "\tsub rax, ", literal->value, "\n", NULL);
							break;
						case BINARY_OPERATOR_MULTIPLY:
							bin_expr_code = straddall(bin_expr_code, "\tmov rbx, ", literal->value, "\n\timul rbx\n", NULL);
							break;
						case BINARY_OPERATOR_DIVIDE:
							bin_expr_code = straddall(bin_expr_code, "\tmov rbx, ", literal->value, "\n\txor rdx, rdx\n\tidiv rbx\n", NULL);
							break;

						case BINARY_OPERATOR_MODULO:
							bin_expr_code = straddall(bin_expr_code, "\tmov rbx, ", literal->value, "\n\txor rdx, rdx\n\tidiv rbx\n\tmov rax, rdx\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
							bin_expr_code = straddall(bin_expr_code, "\tsal rax, ", literal->value, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
							bin_expr_code = straddall(bin_expr_code, "\tsar rax, ", literal->value, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_AND:
							bin_expr_code = straddall(bin_expr_code, "\tand rax, ", literal->value, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_XOR:
							bin_expr_code = straddall(bin_expr_code, "\txor rax, ", literal->value, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_OR:
							bin_expr_code = straddall(bin_expr_code, "\tor rax, ", literal->value, "\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_EQUAL: {
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\txor rcx, rcx\n"
												"\tcmp rax, rbx\n"
												"\tsete cl\n"
												"\tmov rax, rcx\n", NULL);
							break;
						}

						case BINARY_OPERATOR_LOGICAL_NOT_EQUAL: {
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\txor rcx, rcx\n"
												"\tcmp rax, rbx\n"
												"\tsetne cl\n"
												"\tmov rax, rcx\n", NULL);
							break;
						}

						case BINARY_OPERATOR_LOGICAL_GREATER:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\tcmp rax, rbx\n"
												"\tsetg al\n"
												"\tmovzx rax, al\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\txor rcx, rcx\n"
												"\tcmp rax, rbx\n"
												"\tsetge cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_LESS:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\txor rcx, rcx\n"
												"\tcmp rax, rbx\n"
												"\tsetl cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov rbx, ", literal->value, "\n"
												"\txor rcx, rcx\n"
												"\tcmp rax, rbx\n"
												"\tsetle cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						// case BINARY_OPERATOR_LOGICAL_AND:
						// 	bin_expr_code = straddall(bin_expr_code, "and ", reg_a, ", ", var_pointer, "\n", NULL);
						// 	break;

						// case BINARY_OPERATOR_LOGICAL_OR:
						// 	bin_expr_code = straddall(bin_expr_code, "or ", reg_a, ", ", var_pointer, "\n", NULL);
						// 	break;

						default:
							log_error("[3.1] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
							free(bin_expr_code);
							return NULL;
					} // switch (bin_expr->operator)
					break;
				} // case LITERAL_NUMBER
				case LITERAL_IDENTIFIER: {
					if (!scope_contains_variable(scope, literal->value)) {
						log_error("#1 undefined variable '%s'\n", literal->value);
						free(bin_expr_code);
						return NULL;
					}

					VariableTemplate *var_template = scope_get_variable_by_name(scope, literal->value);

					char *var_pointer = compile_variable_pointer(var_template, scope);
					char *reg_a = register_toString(REGISTER_RAX, var_template->datatype->size);
					char *reg_b = register_toString(REGISTER_RBX, var_template->datatype->size);

					switch (bin_expr->operator) {
						case BINARY_OPERATOR_ADD:
							bin_expr_code = straddall(bin_expr_code, "\tadd ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_SUBTRACT:
							bin_expr_code = straddall(bin_expr_code, "\tsub ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_MULTIPLY:
							bin_expr_code = straddall(bin_expr_code, "\tmov ", reg_b, ", ", var_pointer, "\n\timul ", reg_b, "\n", NULL);
							break;

						case BINARY_OPERATOR_DIVIDE:
							bin_expr_code = straddall(bin_expr_code, "\txor rbx, rbx\n\tmov ", reg_b, ", ", var_pointer, "\n\txor rdx, rdx\n\tidiv rbx\n", NULL);
							break;

						case BINARY_OPERATOR_MODULO:
							bin_expr_code = straddall(bin_expr_code, "\tmov ", reg_b, ", ", var_pointer, "\n\txor rdx, rdx\n\tidiv rbx\n\tmov rax, rdx\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
							bin_expr_code = straddall(bin_expr_code, "\tsal ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
							bin_expr_code = straddall(bin_expr_code, "\tsar ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_AND:
							bin_expr_code = straddall(bin_expr_code, "\tand ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_XOR:
							bin_expr_code = straddall(bin_expr_code, "\txor ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_BITWISE_OR:
							bin_expr_code = straddall(bin_expr_code, "\tor ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_EQUAL: {
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\txor rcx, rcx\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsete cl\n"
												"\tmov rax, rcx\n", NULL);
							break;
						}

						case BINARY_OPERATOR_LOGICAL_NOT_EQUAL: {
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\txor rcx, rcx\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsetne cl\n"
												"\tmov rax, rcx\n", NULL);
							break;
						}

						case BINARY_OPERATOR_LOGICAL_GREATER:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsetg cl\n"
												"\tmovzx rax, cl\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\txor rcx, rcx\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsetge cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_LESS:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\txor rcx, rcx\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsetl cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL:
							bin_expr_code = straddall(bin_expr_code, 
												"\tmov ", reg_b, ", ", var_pointer, "\n"
												"\txor rcx, rcx\n"
												"\tcmp ", reg_a, ",",  reg_b, "\n"
												"\tsetle cl\n"
												"\tmov rax, rcx\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_AND:
							bin_expr_code = straddall(bin_expr_code, "\tand ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						case BINARY_OPERATOR_LOGICAL_OR:
							bin_expr_code = straddall(bin_expr_code, "\tor ", reg_a, ", ", var_pointer, "\n", NULL);
							break;

						default:
							log_error("[3] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
							free(bin_expr_code);
							free(var_pointer);
							free(reg_a);
							free(reg_b);
							return NULL;
					} // switch (bin_expr->operator)
					break;
				} // case LITERAL_IDENTIFIER
				default:
					log_error("[4] compile_binary_expression(): literal type '%s' is not supported yet\n", LITERAL_TYPES[literal->type]);
					exit(1);
			} // switch (literal->type)
			break;
 		}

		case EXPRESSION_TYPE_BINARY:
		case EXPRESSION_TYPE_NESTED: 
		case EXPRESSION_TYPE_FUNCTIONCALL:
		case EXPRESSION_TYPE_MEMBERACCESS: {
			char *right_expr_code = compile_expression(bin_expr->expression_right, scope);

			// log_debug("right_expr_code = %s\n", right_expr_code);

			bin_expr_code = straddall(bin_expr_code, 
					"\tpush rax\n",
					right_expr_code,
					"\tmov rbx, rax\n",
					"\tpop rax\n", NULL);

			switch (bin_expr->operator) {
				case BINARY_OPERATOR_ADD:
					bin_expr_code = stradd(bin_expr_code, "\tadd rax, rbx\n");
					break;

				case BINARY_OPERATOR_SUBTRACT:
					bin_expr_code = stradd(bin_expr_code, "\tsub rax, rbx\n");
					break;

				case BINARY_OPERATOR_MULTIPLY:
					bin_expr_code = stradd(bin_expr_code, "\timul rbx\n");
					break;

				case BINARY_OPERATOR_DIVIDE:
					bin_expr_code = stradd(bin_expr_code, "\tidiv rbx\n");
					break;

				case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
					bin_expr_code = stradd(bin_expr_code, "\tsal rax, rbx\n");
					break;

				case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
					bin_expr_code = stradd(bin_expr_code, "\tsar rax, rbx\n");
					break;

				case BINARY_OPERATOR_BITWISE_AND:
					bin_expr_code = stradd(bin_expr_code, "\tand rax, rbx\n");
					break;

				case BINARY_OPERATOR_BITWISE_XOR:
					bin_expr_code = stradd(bin_expr_code, "\txor rax, rbx\n");
					break;

				case BINARY_OPERATOR_BITWISE_OR:
					bin_expr_code = stradd(bin_expr_code, "\tor rax, rbx\n");
					break;

				case BINARY_OPERATOR_LOGICAL_EQUAL:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsete al\n", NULL);
					break;
				case BINARY_OPERATOR_LOGICAL_NOT_EQUAL:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsetne al\n", NULL);
					break;
				case BINARY_OPERATOR_LOGICAL_GREATER:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsetg al\n", NULL);
					break;
				case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsetge al\n", NULL);
					break;
				case BINARY_OPERATOR_LOGICAL_LESS:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsetl al\n", NULL);
					break;

				case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL:
					bin_expr_code = straddall(bin_expr_code,
										"\tcmp rax, rbx\n"
										"\tsetle al\n", NULL);
					break;

				case BINARY_OPERATOR_LOGICAL_AND:
					bin_expr_code = stradd(bin_expr_code, "\tand rax, rbx\n");
					break;

				case BINARY_OPERATOR_LOGICAL_OR:
					bin_expr_code = stradd(bin_expr_code, "\tor rax, rbx\n");
					break;

				default:
					log_error("[5] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
					exit(1);
			} // switch (bin_expr->operator)
			break;
		}

		default:
			log_error("[6] compile_binary_expression(): parsing for expression-type '%d' is not implemented yet\n", bin_expr->expression_right->type);
			exit(1);
	} // switch(bin_expr->expression_right->type)
	return bin_expr_code;
}

char *compile_nested_expression(NestedExpression_T *nested_expr, Scope *scope) {
	return compile_expression(nested_expr->expression, scope);
}

char *compile_literal_expression(Literal_T *literal, Scope *scope) {
	char *literal_expr_code = calloc(1, sizeof(char));

	// log_debug("[6] compile_literal_expression(): literal type '%d' = %s\n", literal->type, literal->value);

	switch (literal->type) {
		case LITERAL_BOOLEAN: // a boolean is also just a number (0 or 1)
		case LITERAL_NUMBER: {
			literal_expr_code = stradd(literal_expr_code, "\tmov rax, ");
			literal_expr_code = stradd(literal_expr_code, literal->value);
			literal_expr_code = stradd(literal_expr_code, "\n");
			break;
		}
		case LITERAL_IDENTIFIER: {
			if (!scope_contains_variable(scope, literal->value)) {
				log_error("#2 undefined variable '%s'\n", literal->value);
				free(literal_expr_code);
				return NULL;
			}

			VariableTemplate *var_template = scope_get_variable_by_name(scope, literal->value);

			// if (var_template->datatype->is_pointer == 1) {
			// 	log_debug("[7] compile_literal_expression(): variable '%s' is a pointer\n", literal->value);
			// } else {
			// 	log_debug("[7] compile_literal_expression(): variable '%s' is NOT a pointer\n", literal->value);
			// }

			literal_expr_code = straddall(literal_expr_code, "\tmov ", 
							register_toString(REGISTER_RAX, var_template->datatype->is_pointer || var_template->datatype->is_array ? 8 : var_template->datatype->size), ", ", 
							compile_variable_pointer(var_template, scope), "\n", NULL);

			break;
		}
		case LITERAL_STRING: {
			printf("    " RED "error: " RESET "string literals are not supported yet\n");
			// TODO(lucalewin): add string literals to global list
			// then add those strings to the global .data section
			break;
		}
		case LITERAL_CHARACTER: {
			literal_expr_code = stradd(literal_expr_code, "\tmov eax, '");
			literal_expr_code = stradd(literal_expr_code, literal->value);
			literal_expr_code = stradd(literal_expr_code, "'\n");
			break;
		}
		// default:
		// 	log_error("unsupported literal type %d\n", literal->type);
		// 	exit(1);
	}

	return literal_expr_code;
}

char *compile_unary_expression(UnaryExpression_T *unary_expr, Scope *scope) {
	char *unary_expr_code = calloc(1, sizeof(char));

	// log_debug("[6] compile_unary_expression(): unary operator '%d'\n", unary_expr->operator);

	switch (unary_expr->operator) {
		case UNARY_OPERATOR_NEGATE: {
			if (unary_expr->identifier->type == LITERAL_NUMBER) {
				unary_expr_code = stradd(unary_expr_code, "\tmov rax, ");
				unary_expr_code = stradd(unary_expr_code, unary_expr->identifier->value);
				unary_expr_code = stradd(unary_expr_code, "\n");
			} else if (unary_expr->identifier->type == LITERAL_IDENTIFIER) {

				unary_expr_code = stradd(unary_expr_code, compile_literal_expression(unary_expr->identifier, scope));

			} else {
				log_error("unary negation with string-literal is not supported\n");
				exit(1);
			}
			unary_expr_code = stradd(unary_expr_code, "\tneg rax\n");
			break;
		}
		case UNARY_OPERATOR_INCREMENT: {
			if (unary_expr->identifier->type != LITERAL_IDENTIFIER) {
				log_error("unary increment with string-literal is not supported\n");
				exit(1);
			}
			VariableTemplate *var_template = scope_get_variable_by_name(scope, unary_expr->identifier->value);
			char *var_pointer = compile_variable_pointer(var_template, scope);
			
			unary_expr_code = straddall(unary_expr_code, 
						"\tinc ", var_pointer, "\n" 
						"\tmov ", register_toString(REGISTER_RAX, var_template->datatype->size), ", ", var_pointer, "\n", NULL);
			break;
		}
		default:
			log_error("compiling for unary operator %d is not supported\n", unary_expr->operator);
			exit(1);
	}

	return unary_expr_code;
}

char *compile_function_call_expression(FunctionCallExpression_T *func_call_expr, Scope *scope) {
	// check if function exists in the current scope
	if (!scope_contains_function(scope, func_call_expr->function_identifier)) {
		log_error("function '%s' does not exist in the current scope\n", func_call_expr->function_identifier);
		return NULL;
	}

	// get the function template of the called function
	FunctionTemplate *func_template = NULL;
	for (size_t i = 0; i < scope->function_templates->size; i++) {
		func_template = arraylist_get(scope->function_templates, i);

		// check if function identifier matches with the one in the function call
		if (strcmp(func_template->identifier, func_call_expr->function_identifier) == 0) {
			// check if parameter count matches
			if (func_template->param_datatypes->size != func_call_expr->argument_expression_list->expressions->size) {
				// parameter count mismatch
				// continue with next function template
				continue;
			}

			// check if parameter types match
			for (size_t j = 0; j < func_template->param_datatypes->size; j++) {
				Datatype *param_dt = arraylist_get(func_template->param_datatypes, j);
				Datatype *arg_dt = arraylist_get(func_call_expr->argument_datatypes, j);

				// compare datatypes
				if (arg_dt == NULL || param_dt == NULL) {
					return NULL;	
				}

				if (datatype_is_number(param_dt) && datatype_is_number(arg_dt)) {
					if (arg_dt->size > param_dt->size) {
						goto next_function;
					}
				} else if (types_equal(param_dt, arg_dt)) {

				} else {
					goto next_function;
				}
			}

			goto found_function;
		}

		next_function: ;
	}

	found_function: ;

	if (func_template == NULL) {
		// no function matching the function call was found
		log_error("function '%s' does not exist in the current scope\n", func_call_expr->function_identifier);
		return NULL;
	}

	char *func_call_expr_code = calloc(1, sizeof(char));

	// log_debug("compile_function_call_expression(): args.count = %d\n", func_call_expr->argument_expression_list->size);

	switch (func_call_expr->argument_expression_list->expressions->size) {
		case 6: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list->expressions, 5), scope), 
					"\tmov r9, rax\n", NULL);
		}
		case 5: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list->expressions, 4), scope), 
					"\tmov r8, rax\n", NULL);
		}
		case 4: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list->expressions, 3), scope), 
					"\tmov rcx, rax\n", NULL);
		}
		case 3: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list->expressions, 2), scope), 
					"\tmov rdx, rax\n", NULL);
		}
		case 2: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list->expressions, 1), scope), 
					"\tmov rsi, rax\n", NULL);
		}
		case 1: {
			Expression_T *expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 0);
			char *expr_code = compile_expression(expr, scope);
			
			func_call_expr_code = straddall(func_call_expr_code,
					expr_code, 
					"\tmov rdi, rax\n", NULL);
			break;
		}
		case 0:
			break;

		default: // more than 6
			log_error("compiling for function call expressions with more than 6 arguments is not supported yet!\n");
			exit(1);
	}

	char *func_lcc_ident = function_to_lcc_identifier(func_template);

	func_call_expr_code = straddall(func_call_expr_code, "\tcall ", func_lcc_ident, "\n", NULL);

	free(func_lcc_ident);

	return func_call_expr_code;
}

char *compile_assignment_expression(AssignmentExpression_T *assignment_expr, Scope *scope) {
	// parse expression on the right hand side of the assignment operator

	// log_debug("compile_assignment_expression(): assignment_expr->identifier = %s\n", assignment_expr->identifier);

	char *assignment_expr_code;
	
	if (assignment_expr->assignment_value->type == EXPRESSION_TYPE_LIST) {
		// TODO(lucalewin): implement
		log_error("compiling for assignment expression with list is not supported yet!\n");
	}
	assignment_expr_code = compile_expression(assignment_expr->assignment_value, scope);

	if (assignment_expr->identifier->type == EXPRESSION_TYPE_LITERAL) {
		// check if literal is an identifier
		// this should always be the case because the parser already checks if this is the case
		// but for extra savety we also check it here
		Literal_T *identifier = assignment_expr->identifier->expr.literal_expr;

		if (identifier->type != LITERAL_IDENTIFIER) {
			log_error("expected identifier but got '%s' instead\n", LITERAL_TYPES[identifier->type]);
			exit(1);
		}

		if (!scope_contains_variable(scope, identifier->value)) {
			log_error("#3 undefined variable '%s'\n", identifier->value);
			free(assignment_expr_code);
			return NULL;
		}

		VariableTemplate *template = scope_get_variable_by_name(scope, identifier->value);

		if (template->is_constant) {
			// throw error: cannot assign to constant
			log_error("cannot assign to constant\n");
			free(assignment_expr_code);
			return NULL;
		}

		char *var_pointer = NULL;

		// char *var_address = scope_get_variable_address(scope, identifier->value);
		if (template->datatype->is_pointer == 1 || template->datatype->is_array == 1) {
			var_pointer = scope_get_variable_address(scope, identifier->value);
			
		} else {
			var_pointer = compile_variable_pointer(template, scope);
		}

		// char *var_pointer = compile_variable_pointer(template, scope);
		char *reg_a = register_toString(REGISTER_RAX, template->datatype->size);
		char *reg_b = register_toString(REGISTER_RBX, template->datatype->size);

		switch (assignment_expr->operator) {
			case ASSIGNMENT_OPERATOR_DEFAULT:
				assignment_expr_code = straddall(assignment_expr_code, "\tmov ", var_pointer, ", ", reg_a, "\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_ADD:
				assignment_expr_code = straddall(assignment_expr_code, "\tadd ", var_pointer, ", ", reg_a, "\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_SUBTRACT:
				assignment_expr_code = straddall(assignment_expr_code, "\tsub ", var_pointer, ", ", reg_a, "\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_MULTIPLY:
				assignment_expr_code = straddall(assignment_expr_code, 
							"\timul ", var_pointer, "\n"
							"\tmov ", var_pointer, ", ", reg_a, "\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_DIVIDE:
				assignment_expr_code = straddall(assignment_expr_code, 
							"\tmov ", reg_b, ", ", reg_a, "\n"
							"\tmov ", reg_a, ", ", var_pointer, "\n"
							"\tidiv ", reg_b, "\n"
							"\tmov ", var_pointer, ", ", reg_a, "\n", NULL);
				break;

			// default:
			// 	log_error("unknown assignment operator: %d\n", assignment_expr->operator);
			// 	exit(1);
		}
	} else if (assignment_expr->identifier->type == EXPRESSION_TYPE_UNARY) {
		log_error("assignment expressions with unary expressions as identifier are not implemented yet\n");
		exit(1);
	} else {
		log_error("unexpected expression for assignment expression identifier\n");
		exit(1);
	}

	return assignment_expr_code;
}

char *compile_array_access_expression(ArrayAccessExpression_T *array_access_expr, Scope *scope) {
	VariableTemplate *template = scope_get_variable_by_name(scope, array_access_expr->identifier->value);

	// for (size_t i = 0; i < scope->global_variable_templates->size; i++) {
	// 	// print name and address of all variables
	// 	VariableTemplate *template = arraylist_get(scope->global_variable_templates, i);
	// 	printf("%s: %s\n", template->identifier, scope_get_variable_address(scope, template->identifier));
	// }

	char *array_access_expr_code = compile_expression(array_access_expr->index_expression, scope);
	const char *dt_directive = _AssemblyDataType_directives[template->datatype->size];
	char *var_address = reference_pointer_variable(template, scope);

	if (template->datatype->size <= 2) {
		if (scope_contains_local_variable(scope, array_access_expr->identifier->value)) {
			// the accessed variable is a local variable
			array_access_expr_code = straddall(array_access_expr_code,
					"\tmov rbx, [", var_address, "]\n"
					"\tmovzx rax, ", dt_directive, "[rbx+", int_to_string(template->datatype->size), "*rax]\n", NULL);
		} else {
			// the accessed variable is a global variable
			array_access_expr_code = straddall(array_access_expr_code, 
					"\tmovzx rax, ", dt_directive, "[", variable_to_lcc_identifier(template), "+", int_to_string(template->datatype->size) ,"*rax]\n", NULL);
		}
	} else {
		if (scope_contains_local_variable(scope, array_access_expr->identifier->value)) {
			// the accessed variable is a local variable
			array_access_expr_code = straddall(array_access_expr_code,
					"\tmov rbx, [", var_address, "]\n"
					"\tmov rax, [rbx+", int_to_string(template->datatype->size), "*rax]\n", NULL);
		} else {
			// the accessed variable is a global variable
			array_access_expr_code = straddall(array_access_expr_code, 
					"\tmov rax, [", variable_to_lcc_identifier(template), "+", int_to_string(template->datatype->size) ,"*rax]\n", NULL);
		}
	}

	return array_access_expr_code;
}

char *compile_member_access_expression(MemberAccessExpression_T *member_access_expr, Scope *scope) {
	char *member_access_expr_code = NULL;
	
	// get location of the member, then put it in the register
	

	return member_access_expr_code;
}

char *compile_list_expression(ExpressionList_T *list_expr, Scope *scope) {
	return "\tnot rax\t; test\n";
}

// ------------------------------------------------------------------------------------------------

char *compile_variable_pointer(VariableTemplate *var_template, Scope *scope) {

	if (!scope_contains_variable(scope, var_template->identifier)) {
		log_error("#4 undefined variable '%s'\n", var_template->identifier);
		return NULL;
	}

	if (scope_contains_local_variable(scope, var_template->identifier)) {
		// the variable is a local variable

		// log_debug("variable '%s' is a local variable\n", var_template->identifier);

		char *var_address = scope_get_variable_address(scope, var_template->identifier);
		if (var_template->datatype->is_pointer || var_template->datatype->is_array) {
			return straddall("QWORD[", var_address, "]", NULL);
		}

		return dereference_pointer_variable(var_template, scope);
	} else {
		// the variable is a global variable which means the var_address is a label
		if (var_template->datatype->is_pointer || var_template->datatype->is_array) {
			// the variable is a global pointer or array which means the var_address is a label
			// Therefore we need to convert the address to an lcc identifier
			return variable_to_lcc_identifier(var_template);
		}
		// the variable is not a pointer or an array and can be directly dereferenced
		return dereference_pointer_variable(var_template, scope);
	}
}

char *dereference_pointer_variable(VariableTemplate *var_template, Scope *scope) {
	char *var_address = NULL;

	if (!scope_contains_variable(scope, var_template->identifier)) {
		log_error("#4 undefined variable '%s'\n", var_template->identifier);
		return NULL;
	}

	if (scope_contains_local_variable(scope, var_template->identifier)) {
		// the variable is a local variable
		var_address = scope_get_variable_address(scope, var_template->identifier);
	} else {
		// the variable is a global variable which means the var_address is a label
		// Therefore we need to convert the address to an lcc identifier
		var_address = variable_to_lcc_identifier(var_template);
	}

	switch (var_template->datatype->size) {
		case 1: // BYTE
			return straddall("BYTE[", var_address, "]", NULL);
		case 2: // WORD
			return straddall("WORD[", var_address, "]", NULL);
		case 4: // DWORD
			return straddall("DWORD[", var_address, "]", NULL);
		case 8: // QWORD
			return straddall("QWORD[", var_address, "]", NULL);
		default:
			log_error("unknown datatype size [#2]: %d (var_name=%s)\n", var_template->datatype->size, var_template->identifier);
			return NULL;
	}
}

char *reference_pointer_variable(VariableTemplate *var_template, Scope *scope) {
	return scope_get_variable_address(scope, var_template->identifier);
}