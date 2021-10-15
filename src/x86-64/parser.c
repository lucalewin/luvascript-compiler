#include <parser.h>
#include <util.h>
#include <logger.h>
#include <statement.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------------ string templates ------------------------

char *header_template =
"section .text\n"
"global _start\n"
"_start:\n\0";

char *exit_template = 
"mov rax, 60\n"
"syscall\n";

// ------------------------ function prototypes ------------------------

char *parse_statement(Statement *stmt);
char *parse_return_statement(ReturnStatement *ret_stmt);
char *parse_expression(Expression_T *expr);
char *parse_binary_expression(BinaryExpression_T *bin_expr);


FILE *stream;
char *stream_buffer;
size_t buffer_length;


char *parse_ast_to_x86_64_asm(AST *root) {
    log_debug("parsing to x68_64 assembly\n");
    char *stmt_code = parse_statement(root->statement);
    char *asm_code = calloc(1, sizeof(char));

    asm_code = stradd(asm_code, header_template);
    asm_code = stradd(asm_code, stmt_code);
    asm_code = stradd(asm_code, exit_template);

    return asm_code;
}

char *parse_statement(Statement *stmt) {
    switch(stmt->type) {
        case STATEMENT_RETURN: {
            return parse_return_statement(stmt->stmt.return_statement);
        }
        default: {
            log_error("parsing for statement type '%d' not implemented yet\n", stmt->type);
            exit(1);
        }
    }
}

char *parse_return_statement(ReturnStatement *ret_stmt) {
    char *ret_stmt_code = calloc(1, sizeof(char));
    char *expr_code = parse_expression(ret_stmt->return_expression);
    ret_stmt_code = stradd(ret_stmt_code, expr_code);
    return stradd(ret_stmt_code, "mov rdi, rax\n");
}

char *parse_expression(Expression_T *expr) {
    char *expr_code;
    switch(expr->type) {
        case EXPRESSION_BINARY:
            expr_code = parse_binary_expression(expr->expr.binary_expr);
            // log_info("expr: %s\n", expr_code);
            break;
        default:
            log_error("parsing for expression type '%d' is not implemented yet\n", expr->type);
            exit(1);
    }
    return expr_code;
}

char *parse_binary_expression(BinaryExpression_T *bin_expr) {
    char *bin_expr_code = calloc(1, sizeof(char));

    switch(bin_expr->expression_left->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_left->expr.literal_expr;

            if (literal->type != LITERAL_NUMBER) {
                log_error("[1] parse_binary_expression(): literal type '%d' is not supported yet\n", literal->type);
            }

            bin_expr_code = stradd(bin_expr_code, "mov rax, ");
            bin_expr_code = stradd(bin_expr_code, literal->value);
            bin_expr_code = stradd(bin_expr_code, "\n");
            
            break;
        }
        case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_left->expr.binary_expr;

            char *nested_bin_expr_code = parse_binary_expression(nested_bin_expr);

            bin_expr_code = stradd(bin_expr_code, nested_bin_expr_code);

            break;
        }
        default:
            log_error("[2] parse_binary_expression(): parsing for expression-type '%d' is not implemented yet\n", bin_expr->expression_left->type);
            exit(1);
    }

    // log_debug("operator: %d\n", bin_expr->operator);

    switch(bin_expr->expression_right->type) {
        case EXPRESSION_LITERAL: {
            Literal_T *literal = bin_expr->expression_right->expr.literal_expr;

            if (literal->type != LITERAL_NUMBER) {
                log_error("[3] parse_binary_expression(): literal type '%d' is not supported yet\n", literal->type);
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
                log_error("[4] parse_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
                exit(1);
            }

            break;
        }
        case EXPRESSION_BINARY: {
            BinaryExpression_T *nested_bin_expr = bin_expr->expression_right->expr.binary_expr;

            char *nested_bin_expr_code = parse_binary_expression(nested_bin_expr);

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
                log_error("[4] parse_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
                exit(1);
            }

            break;
        }
        case EXPRESSION_NESTED: {
            NestedExpression_T *nested_expr = bin_expr->expression_right->expr.nested_expr;
            char *nested_expr_code = parse_expression(nested_expr->expression);

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
                log_error("[5] parse_binary_expression(): binary operator '%d' not implemented yet\n", bin_expr->operator);
                exit(1);
            }
            break;
        }
        default:
            log_error("[6] parse_binary_expression(): parsing for expression-type '%d' is not implemented yet\n", bin_expr->expression_right->type);
            exit(1);
    }

    // log_error("bin expr: %s\n", bin_expr_code);

    return bin_expr_code;
}