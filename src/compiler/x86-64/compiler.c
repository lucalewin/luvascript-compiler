// standart libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// utility libraries
#include <util/util.h>
#include <util/cmd.h>
#include <logging/logger.h>

// types
#include <types/operator.h>
#include <types/literal.h>
#include <types/expression.h>
#include <types/statement.h>
#include <types/function.h>

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

char *compile_function(Function *function);

char *compile_statement(Statement *stmt);
char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope);
char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope);
char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope);

char *compile_expression(Expression_T *expr, Scope *scope);
char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope);
char *compile_literal_expression(Literal_T *literal, Scope *scope);
char *compile_unary_expression(UnaryExpression_T *unary_expr, Scope *scope);

// --------------------- function implementations ----------------------

void compile_asm_file(char *src_file, char *out_file) {
	// get temp object file name
	char *obj_file = malloc(sizeof(char) * (strlen(src_file) + 2));
	strcpy(obj_file, src_file);
	strcat(obj_file, ".o");

	// compile .asm file using nasm and elf64 format
	exec(assembler, file_format, src_file, "-o", obj_file, NULL);

	// link .o file and create binary file
	exec(linker, "-g", obj_file, out_file, NULL);
}

char *compile_to_x86_64_assembly(AST *root) {
	log_debug("compiling to x68_64 assembly\n");
	
	// char *stmt_code = compile_statement(root->statement);
	char *asm_code = calloc(1, sizeof(char));

	// check if main method is defined
	if (scope_contains_function_name(root->global_scope, "main")) {
		// main function is defined
		asm_code = stradd(asm_code, header_template);
		asm_code = stradd(asm_code, "call function_main\n");
		asm_code = stradd(asm_code, "mov rdi, rax\n");
		asm_code = stradd(asm_code, exit_template);
	}

	for (int i = 0; i < root->functions->size; i++) {
		asm_code = stradd(asm_code, compile_function(arraylist_get(root->functions, i)));
	}

	return asm_code;
}

char *compile_function(Function *function) {
	char *function_code = calloc(1, sizeof(char));

	// log_debug("compile_function(): compiling function '%s'\n", function->identifier);

	function_code = straddall(function_code,
	"global function_", function->identifier,
	"\nfunction_", function->identifier, ":\n",
	compile_compound_statement(function->body->stmt.compound_statement, function->body->local_scope),
	"ret\n", NULL);

	return function_code;
}

char *compile_statement(Statement *stmt) {
	switch(stmt->type) {
		case STATEMENT_RETURN:
			return compile_return_statement(stmt->stmt.return_statement, stmt->parent_scope);

		case STATEMENT_COMPOUND:
			return compile_compound_statement(stmt->stmt.compound_statement, stmt->local_scope);

		case STATEMENT_VARIABLE_DECLARATION:
			return compile_variable_declaration_statement(stmt->stmt.variable_decl, stmt->parent_scope);

        default:
            log_error("compile_statement(): unexpected statement type '%s'\n", STATEMENT_TYPES[stmt->type]);
            exit(1);
    }
}

char *compile_compound_statement(CompoundStatement *compound_stmt, Scope *scope) {
	// log_debug("scope: %s\n", scope);

	// calculate size for stack align
	size_t size_for_stack_align = 0;
	for (int i = 0; i < scope->variables->size; i++) {
		Variable *var = arraylist_get(scope->variables, i);
		size_for_stack_align += var->datatype / 8; // 8 -> size of a byte (8 bits)
	}

	// compile to assembly
	char *compound_stmt_code = calloc(1, sizeof(char));

	if (size_for_stack_align != 0) {
		compound_stmt_code = stradd(compound_stmt_code, "push rbp\n"); // save old value of RBP
		compound_stmt_code = stradd(compound_stmt_code, "mov rbp, rsp\n");
		compound_stmt_code = stradd(compound_stmt_code, "sub rsp, "); // align stack
		compound_stmt_code = stradd(compound_stmt_code, int_to_string(size_for_stack_align)); // size in bytes
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
    char *ret_stmt_code = calloc(1, sizeof(char));
    char *expr_code = compile_expression(ret_stmt->return_expression, scope);
    ret_stmt_code = stradd(ret_stmt_code, expr_code);
	return ret_stmt_code;
	// TODO: reset stack base pointer then `ret`
    // return stradd(ret_stmt_code, "ret\n");
}

char *compile_variable_declaration_statement(VariableDeclarationStatement *var_decl_stmt, Scope *scope) {
	char *var_decl_code = calloc(1, sizeof(char));

	Variable *var = var_decl_stmt->var;	
	// compile default expression
	// TODO: simplify default value assignment
	// if (var->default_value->type == EXPRESSION_LITERAL) {}
	var_decl_code = stradd(var_decl_code, compile_expression(var->default_value, scope));	
	switch (var->datatype) {
		case 8: // BYTE
			break;
		case 16: // WORD
			break;
		case 32: // DWORD
			var_decl_code = stradd(var_decl_code, "mov DWORD[rbp-");
			var_decl_code = stradd(var_decl_code, int_to_string(scope_get_variable_rbp_offset(scope, var->identifier->value)));
			var_decl_code = stradd(var_decl_code, "], eax\n");
			break;
		case 64: // QWORD
			break;	
		default:
			log_error("unknown datatype: %d\n", var->datatype);
			exit(1);
			break;
	}	
	return var_decl_code;
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

        default:
            log_error("parsing for expression type '%d' is not implemented yet\n", expr->type);
            exit(1);
    }
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
					Variable *var = scope_get_variable_by_name(scope, literal->value);

					switch (var->datatype) {
						case 8: // BYTE
							break;
						case 16: // WORD
							break;
						case 32: { // DWORD
							if (bin_expr->operator == BINARY_OPERATOR_PLUS) {
								bin_expr_code = stradd(bin_expr_code, "add eax, DWORD[rbp-");
								bin_expr_code = stradd(bin_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
								bin_expr_code = stradd(bin_expr_code, "]\n");
							} else if (bin_expr->operator == BINARY_OPERATOR_MINUS) {
								bin_expr_code = stradd(bin_expr_code, "sub eax, DWORD[rbp-");
								bin_expr_code = stradd(bin_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
								bin_expr_code = stradd(bin_expr_code, "]\n");
							} else if (bin_expr->operator == BINARY_OPERATOR_MULTIPLY) {
								bin_expr_code = stradd(bin_expr_code, "mov ebx, DWORD[rbp-");
								bin_expr_code = stradd(bin_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
								bin_expr_code = stradd(bin_expr_code, "]\n");
								bin_expr_code = stradd(bin_expr_code, "imul ebx\n");
							} else if (bin_expr->operator == BINARY_OPERATOR_DIVIDE) {
								bin_expr_code = stradd(bin_expr_code, "mov ebx, DWORD[rbp-");
								bin_expr_code = stradd(bin_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
								bin_expr_code = stradd(bin_expr_code, "]\n");
								bin_expr_code = stradd(bin_expr_code, "idiv ebx\n");
							} else {
								log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
								exit(1);
							}
							break;
						}
						case 64: // QWORD
							break;

						default:
							log_error("unknown datatype: %d\n", var->datatype);
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
		case EXPRESSION_NESTED: {
            char *nested_bin_expr_code = compile_expression(bin_expr->expression_right, scope);

			bin_expr_code = straddall(bin_expr_code, "push rax\n", nested_bin_expr_code, "mov rbx, rax\n", "pop rax\n", NULL);

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
			Variable *var = scope_get_variable_by_name(scope, literal->value);
			switch (var->datatype) {
				case 8: // BYTE
					break;
				case 16: // WORD
					break;
				case 32: // DWORD
					literal_expr_code = stradd(literal_expr_code, "mov eax, DWORD[rbp-");
					literal_expr_code = stradd(literal_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
					literal_expr_code = stradd(literal_expr_code, "]\n");
					break;
				case 64: // QWORD
					break;
				default:
					log_error("unknown datatype: %d\n", var->datatype);
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
				Variable *var = scope_get_variable_by_name(scope, unary_expr->identifier->value);
				switch (var->datatype) {
					case 8: // BYTE
						break;
					case 16: // WORD
						break;
					case 32: // DWORD
						unary_expr_code = stradd(unary_expr_code, "mov eax, DWORD[rbp-");
						unary_expr_code = stradd(unary_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, var->identifier->value)));
						unary_expr_code = stradd(unary_expr_code, "]\n");
						break;
					case 64: // QWORD
						break;	
					default:
						log_error("unknown datatype: %d\n", var->datatype);
						exit(1);
						break;
				}
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