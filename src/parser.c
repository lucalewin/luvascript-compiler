#include "include/parser.h"

#include "include/debug.h"

AST *parser_create_ast(ArrayList *tokens) {
    // Statement *root = malloc(sizeof(Statement));
    // root->expr = parser_create_expr(tokens);
    AST *ast = malloc(sizeof(AST));
    ast->root = parser_create_expr(tokens);
    // print_expr(ast->root);
    // printf("\n");
    debugPrintExpression(ast->root, 1);
    return ast;
}

void parser_generate_assembly_from_expr(char *filename, Expr *expr) {
    char *path = malloc(sizeof(filename) + 4);
    sprintf(path, "%s.asm", filename);
    FILE *file = fopen(path, "w");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return;
    }
    fputs("section .data\n", file);       // start of data section

    /* define important / standart fields */
    fputs("STDOUT      equ     1\n", file);
    fputs("sys_write   equ     1\n", file);
    fputs("sys_exit    equ     60\n", file);

    fputs("msg         db      \"filename=", file);
    fputs(filename, file);
    fputs("\", 10, 0\n", file);
    fputs("msgLen      dq      ($ - msg)\n", file);

    fputs("section .text\n", file);       // start of code section
    fputs("global _start\n", file);       // define global entry point
    fputs("_start:\n", file);

    /* print string */
    fputs("    mov     rax, sys_write\n", file);
    fputs("    mov     rdi, STDOUT\n", file);
    fputs("    mov     rsi, msg\n", file);
    fputs("    mov     rdx, qword [msgLen]\n", file);
    fputs("    syscall\n", file);
    fputs("    jmp exit\n", file);

    /* exit function */
    fputs("exit:\n", file);
    fputs("    mov     rax, sys_exit\n", file);
    fputs("    xor     rdi, rdi\n", file);
    fputs("    syscall\n", file);

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------

Expr *parser_create_expr(ArrayList *list) {
    ArrayList *arraylist = arraylist_create();
    
    /* convert literal tokens to literal expressions */
    Token *current;
    ParserObject *obj;
    for (int i = 0; i < list->size; i++) {
        current = arraylist_get(list, i);
        obj = malloc(sizeof(ParserObject));
        switch (current->type) {
            case TOKEN_IDENDIFIER: {
                LiteralExpr *le = literal_expr_create(IDENTIFIER, current->data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = literal_expr_to_expr(le);
                token_free(current);
                break;
            }
            case TOKEN_STRING: {
                LiteralExpr *le = literal_expr_create(STRING, current->data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = literal_expr_to_expr(le);
                token_free(current);
                break;
            }
            case TOKEN_NUMBER: {
                LiteralExpr *le = literal_expr_create(NUMBER, current->data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = literal_expr_to_expr(le);
                token_free(current);
                break;
            }
            default: {
                obj->type = TYPE_TOKEN;
                obj->token = current;
                break;
            }
        }
        arraylist_add(arraylist, obj);
    }

    /* 
     * evaluate parenthesis 
     *
     * At first the whole arraylist is passed as parameter
     * because all tokens can be parsed as if they were in
     * parenthesis themselves
     * 
     */
    return evaluate_parenthesis(arraylist);
}

// ----------------------------------------------------------------------------------------------------------

Expr *evaluate_parenthesis(ArrayList *list) {

    /* evaluate paranthesis 
     *
     * precedence level: 0
     * 
     * operators: ( )
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            if (current->token->type == TOKEN_LPAREN) {
                ArrayList *temp = arraylist_create();
                int open = 1, close = 0, index = i+1;
                do {
                    current = arraylist_get(list, index++);
                    arraylist_add(temp, current);
                    if (current->type < 0) { // end of list reached and one or more parenthesis are not closed -> print error + return null;
                        current = arraylist_get(list, i);
                        printf("\nERROR: Parenthesis at [%d:%d] is never closed!\n", 0, 0);//current->token.line, current->token.pos);
                        return NULL;
                    }
                    if (current->type == TYPE_TOKEN) {
                        if (current->token->type == TOKEN_LPAREN) open++;
                        else if (current->token->type == TOKEN_RPAREN) close++;
                    }
                } while (open > close);
                arraylist_remove_at_index(temp, temp->size - 1);
                
                Expr *temp_expr = evaluate_parenthesis(temp);

                ParserObject *obj = malloc(sizeof(ParserObject));
                obj->type = TYPE_EXPRESSION;
                obj->expr = temp_expr;

                arraylist_set_at_index(list, i, obj, 1);   // replace first paren with new Expr struct

                for (int j = index - 1; j > i; j--) {
                    arraylist_remove_at_index(list, j);     // remove tokens from inside the parenthesis
                }
            }
        }
    }

    if (list->capacity > list->size + 16) {
        arraylist_shrink_capacity_to_size(list);
    }

    /*
     * precedence level: 1
     * 
     * operators: 
     *   ++  suffix increment
     *   --  suffix decrement
     * associativity: left-to-right
     * 
     */
    // for (int i = 0; i < list->size; i++) {
    //     ParserObject *current = arraylist_get(list, i);
    //     if (current->type == TYPE_TOKEN) {
    //         switch (current->token->type) {
    //             case TOKEN_INCREMENT:
    //             case TOKEN_DECREMENT: {
    //                 ParserObject *previous = arraylist_get(list, i - 1);
    //                 if (previous->type != TYPE_EXPRESSION) {
    //                     break;   // break, because it could be a prefix increment/decrement -> throw no error
    //                 }

    //                 /*
    //                  *
    //                  * Todo
    //                  *  - create suffix increment/decrement expression struct
    //                  *  - implement methods to create newly created expression
    //                  * 
    //                  */

    //                 // arraylist_set_at_index(list, i-1, obj, 0);    // replace ++ token with unary expression
    //                 // arraylist_remove_at_index(list, i+1);       // remove literal expression
    //                 // i--;
    //                 break;
    //             }
    //             default:
    //                 break;
    //         }
    //     }
    // }

    if (list->capacity > list->size + 16) {
        arraylist_shrink_capacity_to_size(list);
    }

    /*
     * precedence level: 2
     * 
     * operators: 
     *   ++  prefix increment
     *   --  prefix decrement
     * associativity: right-to-left
     * 
     */
    for (int i = list->size - 1; i >= 0; i--) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_INCREMENT:
                case TOKEN_DECREMENT: {
                    ParserObject *next = arraylist_get(list, i + 1);
                    if (next->type != TYPE_EXPRESSION) {
                        printf("ERROR #21: expected expression at [%d:%d]\n", next->token->line, next->token->pos);
                        printf("TYPE: %d\n", next->type);
                        return NULL;
                    }
                    UnaryExpr *unary_expr = unary_expr_create(current->token->data, next->expr);
                    free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                            therefor it doesnt need to be freed */
                    Expr *expr = unary_expr_to_expr(unary_expr);
                    ParserObject *obj = malloc(sizeof(ParserObject));
                    obj->type = TYPE_EXPRESSION;
                    obj->expr = expr;
                    arraylist_set_at_index(list, i, obj, 0);    // replace ++ token with unary expression
                    arraylist_remove_at_index(list, i+1);       // remove literal expression
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (list->capacity > list->size + 16) {
        arraylist_shrink_capacity_to_size(list);
    }

    /*
     * precedence level: 3
     * 
     * operators: 
     *   ** power
     * associativity: right-to-left
     * 
     */
    for (int i = list->size - 1; i >= 0; i--) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            if (current->token->type == TOKEN_POWER) {
                ParserObject *previous = arraylist_get(list, i - 1);
                ParserObject *next = arraylist_get(list, i + 1);
                if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                	printf("ERROR #31: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                	return NULL;
                }
                BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                         therefor it doesnt need to be freed */
                Expr *expr = binary_expr_to_expr(bin_expr);
                ParserObject *obj = malloc(sizeof(ParserObject));
                obj->type = TYPE_EXPRESSION;
                obj->expr = expr;
                arraylist_set_at_index(list, i-1, obj, 0);
                arraylist_remove_at_index(list, i+1);
                arraylist_remove_at_index(list, i--);
            }
        }
    }

    if (list->capacity > list->size + 16) {
        arraylist_shrink_capacity_to_size(list);
    }

    /*
     * precedence level: 4
     * 
     * operators:
     *   *  multiplication
     *   /  division
     *   %  modulo
     * 
     * associativity: left-to-right
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_ASTERISK:
                case TOKEN_SLASH:
                case TOKEN_MOD: {
                    if (i > 0 && i < list->size - 1) {
                        ParserObject *previous = arraylist_get(list, i - 1);
                        ParserObject *next = arraylist_get(list, i + 1);
                        if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                        	printf("ERROR #41: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                        	return NULL;
                        }
                        BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                        free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                                 therefor it doesnt need to be freed */
                        Expr *expr = binary_expr_to_expr(bin_expr);
                        ParserObject *obj = malloc(sizeof(ParserObject));
                        obj->type = TYPE_EXPRESSION;
                        obj->expr = expr;
                        arraylist_set_at_index(list, i-1, obj, 0);
                        arraylist_remove_at_index(list, i+1);
                        arraylist_remove_at_index(list, i--);
                    } else {
                        printf("ERROR #42\n");
                        return NULL;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (list->capacity > list->size + 16) {
        arraylist_shrink_capacity_to_size(list);
    }

    /*
     * precedence level: 5
     * 
     * operators:
     *   +  addition / string concatenation
     *   -  subtraction
     * 
     * associativity: left-to-right
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_PLUS:
                case TOKEN_MINUS: {
                    if (i > 0 && i < list->size - 1) {
                        ParserObject *previous = arraylist_get(list, i - 1);
                        ParserObject *next = arraylist_get(list, i + 1);
                        if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                        	printf("ERROR #51: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                        	return NULL;
                        }
                        BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                        free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                                 therefor it doesnt need to be freed */
                        Expr *expr = binary_expr_to_expr(bin_expr);
                        ParserObject *obj = malloc(sizeof(ParserObject));
                        obj->type = TYPE_EXPRESSION;
                        obj->expr = expr;
                        arraylist_set_at_index(list, i-1, obj, 0);
                        arraylist_remove_at_index(list, i+1);
                        arraylist_remove_at_index(list, i--);
                    } else {
                    	printf("ERROR #52\n");
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    /*
     * precedence level: 7
     * 
     * operators:
     *   <  
     *   <= 
     *   >  
     *   >= 
     * 
     * associativity: left-to-right
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_RELATIONAL_LESS:
                case TOKEN_RELATIONAL_LESS_OR_EQUAL:
                case TOKEN_RELATIONAL_GREATER:
                case TOKEN_RELATIONAL_GREATER_OR_EQUAL: {
                    ParserObject *previous = arraylist_get(list, i - 1);
                        ParserObject *next = arraylist_get(list, i + 1);
                        if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                        	printf("ERROR #51: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                        	return NULL;
                        }
                        BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                        free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                                 therefor it doesnt need to be freed */
                        Expr *expr = binary_expr_to_expr(bin_expr);
                        ParserObject *obj = malloc(sizeof(ParserObject));
                        obj->type = TYPE_EXPRESSION;
                        obj->expr = expr;
                        arraylist_set_at_index(list, i-1, obj, 0);
                        arraylist_remove_at_index(list, i+1);
                        arraylist_remove_at_index(list, i--);
                    break;
                }
                default:
                    break;
            }
        }
    }

    /*
     * precedence level: 8
     * 
     * operators:
     *   ==
     *   !=
     * 
     * associativity: left-to-right
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_RELATIONAL_EQUAL:
                case TOKEN_RELATIONAL_NOT_EQUAL: {
                    ParserObject *previous = arraylist_get(list, i - 1);
                    ParserObject *next = arraylist_get(list, i + 1);
                    if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                    	printf("ERROR #51: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                    	return NULL;
                    }
                    BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                    free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                             therefor it doesnt need to be freed */
                    Expr *expr = binary_expr_to_expr(bin_expr);
                    ParserObject *obj = malloc(sizeof(ParserObject));
                    obj->type = TYPE_EXPRESSION;
                    obj->expr = expr;
                    arraylist_set_at_index(list, i-1, obj, 0);
                    arraylist_remove_at_index(list, i+1);
                    arraylist_remove_at_index(list, i--);
                    break;
                }
                default:
                    break;
            }
        }
    }

    /*
     * precedence level: 9
     * 
     * operators:
     *   &
     * 
     * associativity: left-to-right
     * 
     */
    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_AMPERSAND: {
                    ParserObject *previous = arraylist_get(list, i - 1);
                    ParserObject *next = arraylist_get(list, i + 1);
                    if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                    	printf("ERROR #51: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                    	return NULL;
                    }
                    BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                    free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                             therefor it doesnt need to be freed */
                    Expr *expr = binary_expr_to_expr(bin_expr);
                    ParserObject *obj = malloc(sizeof(ParserObject));
                    obj->type = TYPE_EXPRESSION;
                    obj->expr = expr;
                    arraylist_set_at_index(list, i-1, obj, 0);
                    arraylist_remove_at_index(list, i+1);
                    arraylist_remove_at_index(list, i--);
                    break;
                }
                default:
                    break;
            }
        }
    }

    /*
     * precedence level: 15
     * 
     * operators: 
     *   =
     *   += 
     *   -=
     *   *= 
     *   /= 
     *   %=
     *   <<= 
     *   >>=
     *   &= 
     *   ^= 
     *   |=
     * 
     * associativity: right-to-left
     * 
     */
    for (int i = list->size - 1; i >= 0; i--) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            switch (current->token->type) {
                case TOKEN_ASSIGNMENT_SIMPLE:
                case TOKEN_ASSIGNMENT_SUM:
                case TOKEN_ASSIGNMENT_DIFFERENCE:
                case TOKEN_ASSIGNMENT_PRODUCT:
                case TOKEN_ASSIGNMENT_QUOTIENT:
                case TOKEN_ASSIGNMENT_REMAINDER:
                case TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT:
                case TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT:
                case TOKEN_ASSIGNMENT_BITWISE_AND:
                case TOKEN_ASSIGNMENT_BITWISE_XOR:
                case TOKEN_ASSIGNMENT_BITWISE_OR: {
                    ParserObject *previous = arraylist_get(list, i - 1);
                    ParserObject *next = arraylist_get(list, i + 1);
                    if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
                    	printf("ERROR #151: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                    	return NULL;
                    }
                    BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                    free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                             therefor it doesnt need to be freed */
                    Expr *expr = binary_expr_to_expr(bin_expr);
                    ParserObject *obj = malloc(sizeof(ParserObject));
                    obj->type = TYPE_EXPRESSION;
                    obj->expr = expr;
                    arraylist_set_at_index(list, i-1, obj, 0);
                    arraylist_remove_at_index(list, i+1);
                    arraylist_remove_at_index(list, i--);
                    break;
                }
                default:
                    break;
            }
        }
    }



    if (list->size != 1) {
        printf("ERROR #161\n");
        return NULL;
    }

    ParserObject *current = arraylist_get(list, 0);

    if (current->type != TYPE_EXPRESSION) {
        if (current->type == TYPE_TOKEN) {
            Token *t = current->token;
            printf("ERROR #162: Unexpected token at [%d:%d]: %s\n", t->line, t->pos, t->data);
            return NULL;
        }
        printf("\nERROR #163: Encountered unknown type: %d\n", current->type);
        printf("    This is an compiler error!\n");
        printf("    Please create an issue on the luvascript-compiler github repo:\n");
        printf("    https://github.com/lucr4ft/luvascript-compiler \n\n");
        return NULL;
    }

    if (current->expr == NULL) {
        printf("\nERROR #164: Expression was NULL!\n");
        printf("    This is an compiler error!\n");
        printf("    Please create an issue on the luvascript-compiler github repo:\n");
        printf("    https://github.com/lucr4ft/luvascript-compiler \n\n");
    }

    arraylist_free(list);

    return current->expr;
}
