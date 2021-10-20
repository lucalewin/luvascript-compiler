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

// ----------------------- function prototypes -------------------------

char *compile_statement(Statement *stmt);
char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope);
char *compile_expression(Expression_T *expr, Scope *scope);
char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope);

// --------------------- function implementations ----------------------

char *assembler = "nasm";
char *file_format = "-f elf64";
char *linker = "ld";

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
	
	char *stmt_code = compile_statement(root->statement);
	char *asm_code = calloc(1, sizeof(char));

	asm_code = stradd(asm_code, header_template);
	asm_code = stradd(asm_code, stmt_code);
	asm_code = stradd(asm_code, exit_template);

	return asm_code;
}

char *compile_statement(Statement *stmt) {
	switch(stmt->type) {
		case STATEMENT_RETURN: {
			ReturnStatement *ret_stmt = stmt->stmt.return_statement;
			char *ret_stmt_code = calloc(1, sizeof(char));
			char *expr_code = compile_expression(ret_stmt->return_expression, stmt->parent_scope);
			ret_stmt_code = stradd(ret_stmt_code, expr_code);
			return stradd(ret_stmt_code, "mov rdi, rax\n");
		}

		case STATEMENT_COMPOUND: {
			CompoundStatement *compound_stmt = stmt->stmt.compound_statement;
			// this should act like a function for now until compiling for functions is implemented

			// calculate size for stack align
			Scope *local_scope = stmt->local_scope;
			size_t size_for_stack_align = 0;
			for (int i = 0; i < local_scope->variables->size; i++) {
				Variable *var = arraylist_get(local_scope->variables, i);
				size_for_stack_align += var->datatype / 8; // 8 -> size of a byte (8 bits)
			}

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

		case STATEMENT_VARIABLE_DECLARATION: {
			char *var_decl_code = calloc(1, sizeof(char));

			Scope *local_scope = stmt->parent_scope;
			Variable *var = stmt->stmt.variable_decl->var;

			// compile default expression
			// TODO: simplify default value assignment
			// if (var->default_value->type == EXPRESSION_LITERAL) {}
			var_decl_code = stradd(var_decl_code, compile_expression(var->default_value, local_scope));

			switch (var->datatype) {
				case 8: // BYTE
					break;
				case 16: // WORD
					break;
				case 32: // DWORD
					var_decl_code = stradd(var_decl_code, "mov DWORD[rbp-");
					var_decl_code = stradd(var_decl_code, int_to_string(scope_get_variable_rbp_offset(local_scope, var->identifier->value)));
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

        default: {
            log_error("compile_statement(): unexpected statement type '%s'\n", STATEMENT_TYPES[stmt->type]);
            exit(1);
        }
    }
}

char *compile_return_statement(ReturnStatement *ret_stmt, Scope *scope) {
    char *ret_stmt_code = calloc(1, sizeof(char));
    char *expr_code = compile_expression(ret_stmt->return_expression, scope);
    ret_stmt_code = stradd(ret_stmt_code, expr_code);
    return stradd(ret_stmt_code, "mov rdi, rax\n");
}

char *compile_expression(Expression_T *expr, Scope *scope) {
    char *expr_code;

	// try to simplify the expression
	expr = simplify_expression(expr);

    switch(expr->type) {
		case EXPRESSION_LITERAL: {
			expr_code = calloc(1, sizeof(char));

			Literal_T *literal = expr->expr.literal_expr;

			switch (literal->type) {
				case LITERAL_NUMBER: {
					expr_code = stradd(expr_code, "mov rax, ");
					expr_code = stradd(expr_code, expr->expr.literal_expr->value);
					expr_code = stradd(expr_code, "\n");
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
							expr_code = stradd(expr_code, "mov eax, DWORD[rbp-");
							expr_code = stradd(expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
							expr_code = stradd(expr_code, "]\n");
							break;
						case 64: // QWORD
							break;

						default:
							log_error("unknown datatype: %d\n", var->datatype);
							exit(1);
							break;
					}
				}
				default:
					log_error("unsupported literal type %d\n", literal->type);
					exit(1);
			}
			break;
		}
        case EXPRESSION_BINARY:
            expr_code = compile_binary_expression(expr->expr.binary_expr, scope);
            break;
        default:
            log_error("parsing for expression type '%d' is not implemented yet\n", expr->type);
            exit(1);
    }
    return expr_code;
}

char *compile_binary_expression(BinaryExpression_T *bin_expr, Scope *scope) {
    char *bin_expr_code = calloc(1, sizeof(char));

    switch(bin_expr->expression_left->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_left->expr.literal_expr;

			switch (literal->type) {
				case LITERAL_NUMBER: {
					bin_expr_code = stradd(bin_expr_code, "mov rax, ");
					bin_expr_code = stradd(bin_expr_code, literal->value);
					bin_expr_code = stradd(bin_expr_code, "\n");
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
							bin_expr_code = stradd(bin_expr_code, "mov eax, DWORD[rbp-");
							bin_expr_code = stradd(bin_expr_code, int_to_string(scope_get_variable_rbp_offset(scope, literal->value)));
							bin_expr_code = stradd(bin_expr_code, "]\n");
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
            break;
        }
        
		case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_left->expr.binary_expr;
            char *nested_bin_expr_code = compile_binary_expression(nested_bin_expr, scope);
            bin_expr_code = stradd(bin_expr_code, nested_bin_expr_code);
            break;
        }

		case EXPRESSION_UNARY: {
			UnaryExpression_T *unary_expr = bin_expr->expression_left->expr.unary_expr;

			switch (unary_expr->operator) {
				case UNARY_OPERATOR_NEGATE: {
					if (unary_expr->identifier->type == LITERAL_NUMBER) {
						bin_expr_code = stradd(bin_expr_code, "mov rax, ");
						bin_expr_code = stradd(bin_expr_code, unary_expr->identifier->value);
						bin_expr_code = stradd(bin_expr_code, "\nneg rax\n");
					} else {
						log_error("compiling with identifier is not supported\n");
						exit(1);
					}
					break;
				}
				default:
					log_error("compiling for unary operator %d is not supported\n", unary_expr->operator);
					exit(1);
			}
			break;
		}
        default:
            log_error("[2] compile_binary_expression(): parsing for expression-type '%d' is not implemented yet\n", bin_expr->expression_left->type);
            exit(1);
    }

    switch(bin_expr->expression_right->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_right->expr.literal_expr;

			switch (literal->type) {
				case LITERAL_NUMBER: {
					if (bin_expr->operator == BINARY_OPERATOR_PLUS) {
						bin_expr_code = stradd(bin_expr_code, "add rax, ");
						bin_expr_code = stradd(bin_expr_code, literal->value);
						bin_expr_code = stradd(bin_expr_code, "\n");
					} else if (bin_expr->operator == BINARY_OPERATOR_MINUS) {
						bin_expr_code = stradd(bin_expr_code, "sub rax, ");
						bin_expr_code = stradd(bin_expr_code, literal->value);
						bin_expr_code = stradd(bin_expr_code, "\n");
					} else if (bin_expr->operator == BINARY_OPERATOR_MULTIPLY) {
						bin_expr_code = stradd(bin_expr_code, "mov ebx, ");
						bin_expr_code = stradd(bin_expr_code, literal->value);
						bin_expr_code = stradd(bin_expr_code, "\n");
						bin_expr_code = stradd(bin_expr_code, "imul ebx\n");
					} else if (bin_expr->operator == BINARY_OPERATOR_DIVIDE) {
						bin_expr_code = stradd(bin_expr_code, "mov ebx, ");
						bin_expr_code = stradd(bin_expr_code, literal->value);
						bin_expr_code = stradd(bin_expr_code, "\n");
						bin_expr_code = stradd(bin_expr_code, "idiv ebx\n");
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
        case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_right->expr.binary_expr;

            char *nested_bin_expr_code = compile_binary_expression(nested_bin_expr, scope);

            bin_expr_code = stradd(bin_expr_code, "push rax\n");
            bin_expr_code = stradd(bin_expr_code, nested_bin_expr_code);
            bin_expr_code = stradd(bin_expr_code, "mov rbx, rax\n");
            bin_expr_code = stradd(bin_expr_code, "pop rax\n");
            
            if (bin_expr->operator == BINARY_OPERATOR_PLUS) {
                bin_expr_code = stradd(bin_expr_code, "add rax, rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_MINUS) {
                bin_expr_code = stradd(bin_expr_code, "sub rax, rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_MULTIPLY) {
                bin_expr_code = stradd(bin_expr_code, "imul rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_DIVIDE) {
                bin_expr_code = stradd(bin_expr_code, "idiv rbx\n");
            } else {
                log_error("[4] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
                exit(1);
            }

            break;
        }
        case EXPRESSION_NESTED: {
            NestedExpression_T *nested_expr = bin_expr->expression_right->expr.nested_expr;
            char *nested_expr_code = compile_expression(nested_expr->expression, scope);

            bin_expr_code = stradd(bin_expr_code, "push rax\n");
            bin_expr_code = stradd(bin_expr_code, nested_expr_code);
            bin_expr_code = stradd(bin_expr_code, "mov rbx, rax\n");
            bin_expr_code = stradd(bin_expr_code, "pop rax\n");

            if (bin_expr->operator == BINARY_OPERATOR_PLUS) {
                bin_expr_code = stradd(bin_expr_code, "add rax, rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_MINUS) {
                bin_expr_code = stradd(bin_expr_code, "sub rax, rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_MULTIPLY) {
                bin_expr_code = stradd(bin_expr_code, "imul rbx\n");
            } else if (bin_expr->operator == BINARY_OPERATOR_DIVIDE) {
                bin_expr_code = stradd(bin_expr_code, "idiv rbx\n");
            } else {
                log_error("[5] compile_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
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