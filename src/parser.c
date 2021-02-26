#include "include/parser.h"

void parser_create_ast(ArrayList *tokens) {
    Statement *root = malloc(sizeof(Statement));
    root->expr = parser_create_expr(tokens);
    print_expr(root->expr);
    printf("\n");
    free(root->expr);
    free(root);
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

                arraylist_set_at_index(list, i, obj, 0);   // replace first paren with new Expr struct

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
                	printf("ERROR: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
                	return NULL;
                }
                BinaryExpr *bin_expr = binary_expr_create(previous->expr, current->token->data, next->expr);
                free(current->token); /* because token->data is created as string, gcc already knows the strings size 
                                         therefor it doesnt need to be freed */
                Expr *expr = binary_expr_to_expr(bin_expr);
                ParserObject *obj = malloc(sizeof(ParserObject));
                obj->type = TYPE_EXPRESSION;
                obj->expr = expr;
                arraylist_set_at_index(list, i+1, obj, 0);
                arraylist_remove_at_index(list, i-1);
                arraylist_remove_at_index(list, i++);
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
                        	printf("ERROR: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
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
                        printf("ERROR\n");
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
                        	printf("ERROR: expected expression at [%d:%d]\n", previous->token->line, previous->token->pos);
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
                    	printf("ERROR\n");
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (list->size != 1) {
        printf("ERROR #1\n");
        return NULL;
    }

    ParserObject *current = arraylist_get(list, 0);

    if (current->type != TYPE_EXPRESSION) {
        if (current->type == TYPE_TOKEN) {
            Token *t = current->token;
            printf("ERROR #2: Unexpected token at [%d:%d]: %s\n", t->line, t->pos, t->data);
            return NULL;
        }
        printf("\nERROR #3: Encountered unknown type: %d\n", current->type);
        printf("    This is an error in the compiler\n");
        printf("    Please create an issue on the luvascript-compiler github repo:\n");
        printf("    https://github.com/lucr4ft/luvascript-compiler\n\n");
        return NULL;
    }

    if (current->expr == NULL) {
        printf("\nERROR #4: Expression was NULL!\n");
        printf("    This is an error in the compiler\n");
        printf("    Please create an issue on the luvascript-compiler github repo:\n");
        printf("    https://github.com/lucr4ft/luvascript-compiler\n\n");
    }

    arraylist_free(list);

    return current->expr;
}
