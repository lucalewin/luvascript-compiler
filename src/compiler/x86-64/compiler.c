// standart libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility libraries
#include <util/util.h>
#include <util/cmd.h>
#include <logging/logger.h>

#include <scope_impl.h>

// types
#include <types/operator.h>
#include <types/literal.h>
#include <types/expression.h>
#include <types/statement.h>
#include <types/function.h>
#include <types/datatypes.h>

// headers to be implemented
#include <compiler.h>
#include <x86-64/optimizer.h>

// ------------------------ string templates ------------------------

char *header_template =
"section .text\n"
"global _start\n"
"_start:\n\0";

char *exit_template = 
"mov rax, 60\n"
"syscall\n";

char *assembler = "nasm";
char *file_format = "-f elf64";
char *linker = "ld";

// ----------------------- function prototypes -------------------------

char *compile_global_variable(Variable *glob_var);
char *compile_extern_function_templates(FunctionTemplate *func_template);
char *compile_function(Function *function);

char *compile_statement(Statement *stmt);
char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope);
char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope);
char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope);
char *compile_expression_statement(ExpressionStatement *expr_stmt, Scope *scope);

char *compile_expression(Expression_T *expr, Scope *scope);
char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope);
char *compile_literal_expression(Literal_T *literal, Scope *scope);
char *compile_unary_expression(UnaryExpression_T *unary_expr, Scope *scope);
char *compile_function_call_expression(FunctionCallExpression_T *func_call_expr, Scope *scope);
char *compile_assignment_expression(AssignmentExpression_T *assignment_expr, Scope *scope);

// --------------------- function implementations ----------------------

void compile_asm_file(char *src_file, char *out_file) {
	// get temp object file name
	char *obj_file = calloc(1, sizeof(char));
	obj_file = straddall(obj_file, src_file, ".o", NULL);

	// compile .asm file using nasm and elf64 format
	exec(assembler, file_format, src_file, "-o", obj_file, NULL);

	// link .o file and create binary file
	exec(linker, "-g", obj_file, out_file, NULL);
}

char *compile_to_x86_64_assembly(AST *root) {
	log_debug("compiling to x68_64 assembly\n");
	
	// char *stmt_code = compile_statement(root->statement);
	char *asm_code = calloc(1, sizeof(char));

	if (root->global_variables->size > 0) {
		asm_code = stradd(asm_code, "section .data\n");

		for (size_t i = 0; i < root->global_variables->size; i++) {
			asm_code = stradd(asm_code, compile_global_variable(arraylist_get(root->global_variables, i)));
		}
	}

	// check if main method is defined
	if (scope_contains_function(root->global_scope, "main")) {
		// main function is defined
		asm_code = stradd(asm_code, header_template);
		asm_code = stradd(asm_code, "call main\n");
		asm_code = stradd(asm_code, "mov rdi, rax\n");
		asm_code = stradd(asm_code, exit_template);
	} else {
		asm_code = stradd(asm_code, "section .text\n");
	}

	// compile functions
	for (int i = 0; i < root->functions->size; i++) {
		asm_code = stradd(asm_code, compile_function(arraylist_get(root->functions, i)));
	}

	// define external functions
	for (size_t i = 0; i < root->extern_functions->size; i++) {
		asm_code = stradd(asm_code, compile_extern_function_templates(arraylist_get(root->extern_functions, i)));
	}

	return asm_code;
}

char *compile_global_variable(Variable *glob_var) {

	if (glob_var->default_value->type != EXPRESSION_LITERAL) {
		log_error("only global variables with literal expressions are supported yet\n");
		exit(1);
	}

	switch (glob_var->datatype->size) {
		case 1: // BYTE
			return straddall(glob_var->identifier->value, " db ", glob_var->default_value->expr.literal_expr->value, "\n", NULL);

		case 2: // WORD
			return straddall(glob_var->identifier->value, " dw ", glob_var->default_value->expr.literal_expr->value, "\n", NULL);

		case 4: // DWORD
			return straddall(glob_var->identifier->value, " dd ", glob_var->default_value->expr.literal_expr->value, "\n", NULL);

		case 8: // QWORD
			return straddall(glob_var->identifier->value, " dq ", glob_var->default_value->expr.literal_expr->value, "\n", NULL);
			
		default:
			log_error("unknown variable size\n");
			exit(1);
	}
}

char *compile_extern_function_templates(FunctionTemplate *func_template) {
	return straddall("extern ", func_template->identifier, "\n", NULL);
}

char *compile_function(Function *function) {
	char *function_code = calloc(1, sizeof(char));

	// log_debug("compile_function(): compiling function '%s'\n", function->identifier);

	function_code = straddall(function_code,
	"global ", function->identifier,
	"\n", function->identifier, ":\n", NULL);

	// align stack

	size_t size_for_stack_align = 0;
	for (int i = 0; i < function->scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(function->scope->local_variable_templates, i);
		size_for_stack_align += var_template->datatype->size;
	}

	if (size_for_stack_align != 0) {
		function_code = stradd(function_code, "push rbp\n"); // save old value of RBP
		function_code = stradd(function_code, "mov rbp, rsp\n");
		function_code = stradd(function_code, "sub rsp, "); // align stack
		function_code = stradd(function_code, int_to_string(size_for_stack_align + 8)); // size in bytes
		function_code = stradd(function_code, "\n");
		// log_debug("aligning stack by %d bytes\n", size_for_stack_align);
	}

	// compile args

	switch (function->parameters->size) {
		case 6: {
			Variable *param = arraylist_get(function->parameters, 5);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], r9\n");
		}
		case 5: {
			Variable *param = arraylist_get(function->parameters, 4);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], r8\n");
		}
		case 4: {
			Variable *param = arraylist_get(function->parameters, 3);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], ecx\n");
		}
		case 3: {
			Variable *param = arraylist_get(function->parameters, 2);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], edx\n");
		}
		case 2: {
			Variable *param = arraylist_get(function->parameters, 1);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], esi\n");
		}
		case 1: {
			Variable *param = arraylist_get(function->parameters, 0);
			function_code = stradd(function_code, "mov DWORD[rbp-");
			function_code = stradd(function_code, int_to_string(scope_get_variable_rbp_offset(function->scope, param->identifier->value)));
			function_code = stradd(function_code, "], edi\n");
		}
		case 0:
			break;
		default:
			break;
	}

	// compile statements
	char *stmts_code = calloc(1, sizeof(char));
	for (size_t i = 0; i < function->body_statements->size; i++) {
		Statement *stmt = arraylist_get(function->body_statements, i);
		stmts_code = stradd(stmts_code, compile_statement(stmt));
	}
	function_code = stradd(function_code, stmts_code);

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

		default:
			log_error("compile_statement(): unexpected statement type '%s'\n", STATEMENT_TYPES[stmt->type]);
			exit(1);
	}
}

char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope) {
	// calculate size for stack align
	size_t size_for_stack_align = 0;
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(scope->local_variable_templates, i);
		size_for_stack_align += var_template->datatype->size;
	}

	// compile to assembly
	char *compound_stmt_code = calloc(1, sizeof(char));

	if (size_for_stack_align != 0) {
		compound_stmt_code = stradd(compound_stmt_code, "push rbp\n"); // save old value of RBP
		compound_stmt_code = stradd(compound_stmt_code, "mov rbp, rsp\n");
		compound_stmt_code = stradd(compound_stmt_code, "sub rsp, "); // align stack
		compound_stmt_code = stradd(compound_stmt_code, int_to_string(size_for_stack_align + 8)); // size in bytes
		compound_stmt_code = stradd(compound_stmt_code, "\n");
	}

	for (int i = 0; i < compound_stmt->nested_statements->size; i++) {
		compound_stmt_code = stradd(compound_stmt_code, compile_statement(arraylist_get(compound_stmt->nested_statements, i)));
	}

	if (size_for_stack_align != 0) {
		compound_stmt_code = stradd(compound_stmt_code, "mov rsp, rbp\n");
		compound_stmt_code = stradd(compound_stmt_code, "pop rbp\n");
	}

	return compound_stmt_code;
}

char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope) {
	// log_debug("before expression compilation\n");

	char *ret_stmt_code = compile_expression(ret_stmt->return_expression, scope);

	// log_debug("after expression compilation\n");

	// reset base pointer
	size_t size_for_stack_align = 0;
	for (int i = 0; i < scope->local_variable_templates->size; i++) {
		VariableTemplate *var_template = arraylist_get(scope->local_variable_templates, i);
		size_for_stack_align += var_template->datatype->size;
	}

	if (size_for_stack_align != 0) {
		ret_stmt_code = stradd(ret_stmt_code, "mov rsp, rbp\npop rbp\n");
	}

	ret_stmt_code = stradd(ret_stmt_code, "ret\n");

	return ret_stmt_code;
}

char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope) {
	// log_debug("compile_variable_declaration_statement():\n");

	Variable *var = var_decl_stmt->var;	

	// compile default expression
	// TODO: simplify default value assignment
	// if (var->default_value->type == EXPRESSION_LITERAL) {}

	char *var_decl_code = compile_expression(var->default_value, scope);
	char *rbp_offset = int_to_string(scope_get_variable_rbp_offset(scope, var->identifier->value));

	switch (var->datatype->size) {
		case 1: // BYTE
			var_decl_code = straddall(var_decl_code, "mov BYTE[rbp-", rbp_offset, "], al\n");
			break;
		case 2: // WORD
			var_decl_code = straddall(var_decl_code, "mov WORD[rbp-", rbp_offset, "], ax\n");
			break;
		case 4: // DWORD
			var_decl_code = straddall(var_decl_code, "mov DWORD[rbp-", rbp_offset, "], eax\n");
			break;
		case 8: // QWORD
			var_decl_code = straddall(var_decl_code, "mov QWORD[rbp-", rbp_offset, "], rax\n");
			break;
		default:
			log_error("unexpected datatype size: %d\n", var->datatype->size);
			exit(1);
			break;
	}

	return var_decl_code;
}

char *compile_expression_statement(ExpressionStatement *expr_stmt, Scope *scope) {
	char *expr_stmt_code = compile_expression(expr_stmt->expression, scope);

	

	return expr_stmt_code;
}

char *compile_expression(Expression_T *expr, Scope *scope) {
	// try to simplify the expression
	expr = simplify_expression(expr);

	switch(expr->type) {
		case EXPRESSION_LITERAL:
			return compile_literal_expression(expr->expr.literal_expr, scope);

		case EXPRESSION_BINARY:
			return compile_binary_expression(expr->expr.binary_expr, scope);

		case EXPRESSION_UNARY:
			return compile_unary_expression(expr->expr.unary_expr, scope);

		case EXPRESSION_FUNCTION_CALL:
			return compile_function_call_expression(expr->expr.func_call_expr, scope);

		case EXPRESSION_ASSIGNMENT:
			return compile_assignment_expression(expr->expr.assignment_expr, scope);

		default:
			log_error("parsing for expression type '%d' is not implemented yet\n", expr->type);
			exit(1);
	}

	log_error("compile_expression(): returning NULL\n");
	return NULL;
}

char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope) {
	char *bin_expr_code = calloc(1, sizeof(char));

	bin_expr_code = stradd(bin_expr_code, compile_expression(bin_expr->expression_left, scope));

	switch(bin_expr->expression_right->type) {
		case EXPRESSION_LITERAL: {
			Literal_T *literal = bin_expr->expression_right->expr.literal_expr;

			switch (literal->type) {
				case LITERAL_NUMBER: {
					if (bin_expr->operator == BINARY_OPERATOR_PLUS) {
						bin_expr_code = straddall(bin_expr_code, "add rax, ", literal->value, "\n", NULL);
					} else if (bin_expr->operator == BINARY_OPERATOR_MINUS) {
						bin_expr_code = straddall(bin_expr_code, "sub rax, ", literal->value, "\n", NULL);
					} else if (bin_expr->operator == BINARY_OPERATOR_MULTIPLY) {
						bin_expr_code = straddall(bin_expr_code, "mov rbx, ", literal->value, "\nimul rbx\n", NULL);
					} else if (bin_expr->operator == BINARY_OPERATOR_DIVIDE) {
						bin_expr_code = straddall(bin_expr_code, "mov rbx, ", literal->value, "\nidiv rbx\n", NULL);
					} else {
						log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
						exit(1);
					}
					break;
				}
				case LITERAL_IDENTIFIER: {
					if (!scope_contains_variable(scope, literal->value)) {
						log_error("undefined variable '%s'\n", literal->value);
						free(bin_expr_code);
						return NULL;
					}

					VariableTemplate *var_template = scope_get_variable_by_name(scope, literal->value);
					char *var_address = scope_get_variable_address(scope, literal->value);

					switch (var_template->datatype->size) {
						case 1: // BYTE
							switch (bin_expr->operator) {
								case BINARY_OPERATOR_PLUS:
									bin_expr_code = straddall(bin_expr_code, "add al, BYTE[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MINUS:
									bin_expr_code = straddall(bin_expr_code, "sub al, BYTE[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MULTIPLY:
									bin_expr_code = straddall(bin_expr_code, 
										"mov bl, BYTE[", var_address, "]\n"
										"imul bl\n", NULL);
									break;
								case BINARY_OPERATOR_DIVIDE:
									bin_expr_code = straddall(bin_expr_code, 
										"mov bl, BYTE[", var_address, "]\n"
										"idiv bl\n", NULL);
									break;
								default:
									log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
									free(bin_expr_code);
									return NULL;
							}
							break;
						case 2: // WORD
							switch (bin_expr->operator) {
								case BINARY_OPERATOR_PLUS:
									bin_expr_code = straddall(bin_expr_code, "add ax, WORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MINUS:
									bin_expr_code = straddall(bin_expr_code, "sub ax, WORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MULTIPLY:
									bin_expr_code = straddall(bin_expr_code, 
										"mov bx, WORD[", var_address, "]\n"
										"imul bx\n", NULL);
									break;
								case BINARY_OPERATOR_DIVIDE:
									bin_expr_code = straddall(bin_expr_code, 
										"mov bx, WORD[", var_address, "]\n"
										"idiv bx\n", NULL);
									break;
								default:
									log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
									free(bin_expr_code);
									return NULL;
							}
							break;
						case 4: { // DWORD
							switch (bin_expr->operator) {
								case BINARY_OPERATOR_PLUS:
									bin_expr_code = straddall(bin_expr_code, "add eax, DWORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MINUS:
									bin_expr_code = straddall(bin_expr_code, "sub eax, DWORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MULTIPLY:
									bin_expr_code = straddall(bin_expr_code, 
										"mov ebx, DWORD[", var_address, "]\n"
										"imul ebx\n", NULL);
									break;
								case BINARY_OPERATOR_DIVIDE:
									bin_expr_code = straddall(bin_expr_code, 
										"mov ebx, DWORD[", var_address, "]\n"
										"idiv ebx\n", NULL);
									break;
								default:
									log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
									free(bin_expr_code);
									return NULL;
							}
							break;
						}
						case 8: // QWORD
							switch (bin_expr->operator) {
								case BINARY_OPERATOR_PLUS:
									bin_expr_code = straddall(bin_expr_code, "add rax, QWORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MINUS:
									bin_expr_code = straddall(bin_expr_code, "sub rax, QWORD[", var_address, "]\n", NULL);
									break;
								case BINARY_OPERATOR_MULTIPLY:
									bin_expr_code = straddall(bin_expr_code, 
										"mov rbx, QWORD[", var_address, "]\n"
										"imul rbx\n", NULL);
									break;
								case BINARY_OPERATOR_DIVIDE:
									bin_expr_code = straddall(bin_expr_code, 
										"mov rbx, QWORD[", var_address, "]\n"
										"idiv rbx\n", NULL);
									break;
								default:
									log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
									free(bin_expr_code);
									return NULL;
							}
							break;

						default:
							log_error("unknown datatype size: %d\n", var_template->datatype->size);
							exit(1);
							break;
					}
					break;
				}
				default:
					log_error("[4] compile_binary_expression(): literal type '%d' is not supported yet\n", literal->type);
					exit(1);
			}
			break;
 		}

		case EXPRESSION_BINARY:
		case EXPRESSION_NESTED: 
		case EXPRESSION_FUNCTION_CALL: {
			char *right_expr_code = compile_expression(bin_expr->expression_right, scope);

			bin_expr_code = straddall(bin_expr_code, 
					"push rax\n",
					right_expr_code,
					"mov rbx, rax\n",
					"pop rax\n", NULL);

			switch (bin_expr->operator) {
				case BINARY_OPERATOR_PLUS:
					bin_expr_code = stradd(bin_expr_code, "add rax, rbx\n");
					break;

				case BINARY_OPERATOR_MINUS:
					bin_expr_code = stradd(bin_expr_code, "sub rax, rbx\n");
					break;

				case BINARY_OPERATOR_MULTIPLY:
					bin_expr_code = stradd(bin_expr_code, "imul rbx\n");
					break;

				case BINARY_OPERATOR_DIVIDE:
					bin_expr_code = stradd(bin_expr_code, "idiv rbx\n");
					break;

				default:
					log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
					exit(1);
			}
			break;
		}

		default:
			log_error("[6] compile_binary_expression(): parsing for expression-type '%d' is not implemented yet\n", bin_expr->expression_right->type);
			exit(1);
	}

	// log_warning("before returning bin_expr_code\n");
	return bin_expr_code;
}

char *compile_literal_expression(Literal_T *literal, Scope *scope) {
	char *literal_expr_code = calloc(1, sizeof(char));

	switch (literal->type) {
		case LITERAL_NUMBER: {
			literal_expr_code = stradd(literal_expr_code, "mov rax, ");
			literal_expr_code = stradd(literal_expr_code, literal->value);
			literal_expr_code = stradd(literal_expr_code, "\n");
			break;
		}
		case LITERAL_IDENTIFIER: {
			if (!scope_contains_variable(scope, literal->value)) {
				log_error("undefined variable '%s'\n", literal->value);
				free(literal_expr_code);
				return NULL;
			}

			VariableTemplate *var_template = scope_get_variable_by_name(scope, literal->value);
			char *var_address = scope_get_variable_address(scope, literal->value);

			switch (var_template->datatype->size) {
				case 1: // BYTE
					literal_expr_code = straddall(literal_expr_code, "mov al, BYTE[", var_address, "]\n", NULL);
					break;
				case 2: // WORD
					literal_expr_code = straddall(literal_expr_code, "mov ax, WORD[", var_address, "]\n", NULL);
					break;
				case 4: // DWORD
					literal_expr_code = straddall(literal_expr_code, "mov eax, DWORD[", var_address, "]\n", NULL);
					break;
				case 8: // QWORD
					literal_expr_code = straddall(literal_expr_code, "mov rax, QWORD[", var_address, "]\n", NULL);
					break;
				default:
					log_error("unknown datatype size: %d\n", var_template->datatype->size);
					exit(1);
					break;
			}
			break;
		}
		default:
			log_error("unsupported literal type %d\n", literal->type);
			exit(1);
	}

	return literal_expr_code;
}

char *compile_unary_expression(UnaryExpression_T *unary_expr, Scope *scope) {
	char *unary_expr_code = calloc(1, sizeof(char));

	switch (unary_expr->operator) {
		case UNARY_OPERATOR_NEGATE: {
			if (unary_expr->identifier->type == LITERAL_NUMBER) {
				unary_expr_code = stradd(unary_expr_code, "mov rax, ");
				unary_expr_code = stradd(unary_expr_code, unary_expr->identifier->value);
				unary_expr_code = stradd(unary_expr_code, "\n");
			} else if (unary_expr->identifier->type == LITERAL_IDENTIFIER) {

				unary_expr_code = stradd(unary_expr_code, compile_literal_expression(unary_expr->identifier, scope));

			} else {
				log_error("unary negation with string-literal is not supported\n");
				exit(1);
			}
			unary_expr_code = stradd(unary_expr_code, "neg rax\n");
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

	// check if parameter count matches
	// TODO(lucalewin): check if parameter types match
	FunctionTemplate *func_template = scope_get_function_by_name(scope, func_call_expr->function_identifier);
	if (func_template->param_datatypes->size != func_call_expr->argument_expression_list->size) {
		log_error("expected %d arguments for function '%s' but got %d instead\n", 
				func_template->param_datatypes->size, 
				func_template->identifier, 
				func_call_expr->argument_expression_list->size);
		return NULL;
	}

	char *func_call_expr_code = calloc(1, sizeof(char));

	// log_debug("compile_function_call_expression(): args.count = %d\n", func_call_expr->argument_expression_list->size);

	switch (func_call_expr->argument_expression_list->size) {
		case 6: {
			func_call_expr_code = straddall(func_call_expr_code, 
					"push rax\n", 
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 5), scope), 
					"mov r9, rax\n"
					"pop rax\n", NULL);
		}
		case 5: {
			func_call_expr_code = straddall(func_call_expr_code, 
					"push rax\n", 
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 4), scope), 
					"mov r8, rax\n"
					"pop rax\n", NULL);
		}
		case 4: {
			func_call_expr_code = straddall(func_call_expr_code, 
					"push rax\n", 
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 3), scope), 
					"mov rcx, rax\n"
					"pop rax\n", NULL);
		}
		case 3: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 2), scope), 
					"mov rdx, rax\n", NULL);
		}
		case 2: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 1), scope), 
					"mov rsi, rax\n", NULL);
		}
		case 1: {
			func_call_expr_code = straddall(func_call_expr_code,
					compile_expression(arraylist_get(func_call_expr->argument_expression_list, 0), scope), 
					"mov rdi, rax\n", NULL);
			break;
		}
		case 0:
			break;

		default: // more than 6
			log_error("compiling for function call expressions with more than 6 arguments is not supported yet!\n");
			exit(1);
	}

	func_call_expr_code = straddall(func_call_expr_code, "call ", func_call_expr->function_identifier, "\n", NULL);

	return func_call_expr_code;
}

char *compile_assignment_expression(AssignmentExpression_T *assignment_expr, Scope *scope) {
	// parse expression on the right hand side of the assignment operator
	char *assignment_expr_code = compile_expression(assignment_expr->assignment_value, scope);

	if (assignment_expr->identifier->type == EXPRESSION_LITERAL) {
		// check if literal is an identifier
		// this should always be the case because the parser already checks if this is the case
		// but for extra savety we also check it here
		Literal_T *identifier = assignment_expr->identifier->expr.literal_expr;

		if (identifier->type != LITERAL_IDENTIFIER) {
			log_error("expected identifier but got '%s' instead\n", LITERAL_TYPES[identifier->type]);
			exit(1);
		}

		if (!scope_contains_variable(scope, identifier->value)) {
			log_error("undefined variable '%s'\n", identifier->value);
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

		char *var_address = scope_get_variable_address(scope, identifier->value);

		switch (assignment_expr->operator) {
			case ASSIGNMENT_OPERATOR_DEFAULT:
				assignment_expr_code = straddall(assignment_expr_code, "mov DWORD[", var_address, "], eax\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_ADD:
				assignment_expr_code = straddall(assignment_expr_code, "add DWORD[", var_address, "], eax\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_SUBTRACT:
				assignment_expr_code = straddall(assignment_expr_code, "sub DWORD[", var_address, "], eax\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_MULTIPLY:
				assignment_expr_code = straddall(assignment_expr_code, "imul DWORD[", var_address, "]\nmov DWORD[", var_address, "], eax\n", NULL);
				break;

			case ASSIGNMENT_OPERATOR_DIVIDE:
				assignment_expr_code = straddall(assignment_expr_code, 
							"mov rbx, rax\n"
							"mov eax, DWORD[", var_address, "]\n"
							"idiv ebx\n"
							"mov DWORD[", var_address, "], eax\n", NULL);
				break;

			default:
				log_error("unknown assignment operator: %d\n", assignment_expr->operator);
				exit(1);
		}
	} else if (assignment_expr->identifier->type == EXPRESSION_UNARY) {
		log_error("assignment expressions with unary expressions as identifier are not implemented yet\n");
		exit(1);
	} else {
		log_error("unexpected expression for assignment expression identifier\n");
		exit(1);
	}

	return assignment_expr_code;
}