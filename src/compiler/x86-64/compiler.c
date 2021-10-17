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
char *compile_return_statement(ReturnStatement *ret_stmt);
char *compile_expression(Expression_T *expr);
char *compile_binary_expression(BinaryExpression_T *bin_expr);

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
    exec(linker, "-g", obj_file, out_file);
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
            return compile_return_statement(stmt->stmt.return_statement);
        }
		case STATEMENT_COMPOUND: {
			CompoundStatement *compound_statement = stmt->stmt.compound_statement;
			
			char *compound_stmt_code = calloc(1, sizeof(char));

			for (int i = 0; i < compound_statement->nested_statements->size; i++) {
				compound_stmt_code = stradd(compound_stmt_code, compile_statement(arraylist_get(compound_statement->nested_statements, i)));
			}

			break;
		}
        default: {
            log_error("compile_statement(): unexpected statement type '%s'\n", STATEMENT_TYPES[stmt->type]);
            exit(1);
        }
    }
}

char *compile_return_statement(ReturnStatement *ret_stmt) {
    char *ret_stmt_code = calloc(1, sizeof(char));
    char *expr_code = compile_expression(ret_stmt->return_expression);
    ret_stmt_code = stradd(ret_stmt_code, expr_code);
    return stradd(ret_stmt_code, "mov rdi, rax\n");
}

char *compile_expression(Expression_T *expr) {
    char *expr_code;

	expr = simplify_expression(expr);

    switch(expr->type) {
		case EXPRESSION_LITERAL: {
			expr_code = calloc(1, sizeof(char));
			expr_code = stradd(expr_code, "mov rax, ");
			expr_code = stradd(expr_code, expr->expr.literal_expr->value);
			expr_code = stradd(expr_code, "\n");
			break;
		}
        case EXPRESSION_BINARY:
            expr_code = compile_binary_expression(expr->expr.binary_expr);
            break;
        default:
            log_error("parsing for expression type '%d' is not implemented yet\n", expr->type);
            exit(1);
    }
    return expr_code;
}

char *compile_binary_expression(BinaryExpression_T *bin_expr) {
    char *bin_expr_code = calloc(1, sizeof(char));

    switch(bin_expr->expression_left->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_left->expr.literal_expr;

            if (literal->type != LITERAL_NUMBER) {
                log_error("[1] compile_binary_expression(): literal type '%d' is not supported yet\n", literal->type);
            }

            bin_expr_code = stradd(bin_expr_code, "mov rax, ");
            bin_expr_code = stradd(bin_expr_code, literal->value);
            bin_expr_code = stradd(bin_expr_code, "\n");
            
            break;
        }
        
		case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_left->expr.binary_expr;

            char *nested_bin_expr_code = compile_binary_expression(nested_bin_expr);

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

    // log_debug("operator: %d\n", bin_expr->operator);

    switch(bin_expr->expression_right->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_right->expr.literal_expr;

            if (literal->type != LITERAL_NUMBER) {
                log_error("[3] compile_binary_expression(): literal type '%d' is not supported yet\n", literal->type);
            }

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
        case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_right->expr.binary_expr;

            char *nested_bin_expr_code = compile_binary_expression(nested_bin_expr);

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
            char *nested_expr_code = compile_expression(nested_expr->expression);

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

    // log_error("bin expr: %s\n", bin_expr_code);

    return bin_expr_code;
}