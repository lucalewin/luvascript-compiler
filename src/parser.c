#include "include/parser.h"

void parser_create_ast(TokenList *tokens) {
    Statement *root = malloc(sizeof(Statement));
    root->expr = parser_create_expr(tokens);
    free(root->expr);
    free(root);
}

// ----------------------------------------------------------------------------------------------------------

Expr *parser_create_expr(TokenList *list) {
    ArrayList *arraylist = arraylist_create();

    /* convert literal tokens to literal expressions */
    for (int i = 0; i < list->size; i++) {
        ParserObject *obj = malloc(sizeof(ParserObject));
        switch (list->tokens[i].type) {
            case TOKEN_IDENDIFIER: {
                LiteralExpr *le = literal_expr_create(IDENTIFIER, list->tokens[i].data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = *literal_expr_to_expr(le);
                break;
            }
            case TOKEN_STRING: {
                LiteralExpr *le = literal_expr_create(STRING, list->tokens[i].data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = *literal_expr_to_expr(le);
                break;
            }
            case TOKEN_NUMBER: {
                LiteralExpr *le = literal_expr_create(NUMBER, list->tokens[i].data);
                obj->type = TYPE_EXPRESSION;
                obj->expr = *literal_expr_to_expr(le);
                break;
            }
            default:
                obj->type = TYPE_TOKEN;
                obj->token = list->tokens[i];
                break;
        }
        arraylist_add(arraylist, obj);
    }

    /* 
     * evaluate parenthesis 
     *
     * At first the whole arraylist passed as parameter
     * because all tokens can be parsed as if they were
     * in parenthesis themselfs 
     * 
     */
    return evaluate_parenthesis(arraylist);
}

// ----------------------------------------------------------------------------------------------------------

Expr *evaluate_parenthesis(ArrayList *list) {
    printf("------------------------\n");
    printf("evaluating parenthesis\n");

    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            if (current->token.type == TOKEN_LPAREN) {
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
                        if (current->token.type == TOKEN_LPAREN) open++;
                        else if (current->token.type == TOKEN_RPAREN) close++;
                    }
                } while (open > close);
                arraylist_remove_at_index(temp, temp->size - 1);
                
                Expr *temp_expr = evaluate_parenthesis(temp);

                ParserObject *obj = malloc(sizeof(ParserObject));
                obj->type = TYPE_EXPRESSION;
                obj->expr = *temp_expr;

                arraylist_set_at_index(list, i, obj, 0);   // replace first paren with new Expr struct

                for (int j = index - 1; j > i; j--) {
                    arraylist_remove_at_index(list, j);     // remove tokens from inside the parenthesis
                }
                // i = index - 1;
            }
        }
    }

    for (int i = 0; i < list->size; i++) {
    	ParserObject *current = arraylist_get(list, i);
    	if (current->type == TYPE_TOKEN) {
    		switch (current->token.type) {
				case TOKEN_ASTERISK:
				case TOKEN_SLASH:
				case TOKEN_MOD: {
					if (i > 0 && i < list->size - 1) {
						ParserObject *previous = arraylist_get(list, i - 1);
						ParserObject *next = arraylist_get(list, i + 1);
						if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
							printf("ERROR: expected expression at [%d:%d]\n", previous->token.line, previous->token.pos);
							return NULL;
						}
						BinaryExpr *bin_expr = binary_expr_create(&(previous->expr), current->token.data, &(next->expr));
						Expr *expr = binary_expr_to_expr(bin_expr);
						ParserObject *obj = malloc(sizeof(ParserObject));
						obj->type = TYPE_EXPRESSION;
						obj->expr = *expr;
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

    for (int i = 0; i < list->size; i++) {
    	ParserObject *current = arraylist_get(list, i);
    	if (current->type == TYPE_TOKEN) {
    		switch (current->token.type) {
				case TOKEN_PLUS:
				case TOKEN_MINUS: {
					if (i > 0 && i < list->size - 1) {
						ParserObject *previous = arraylist_get(list, i - 1);
						ParserObject *next = arraylist_get(list, i + 1);
						if (previous->type != TYPE_EXPRESSION || next->type != TYPE_EXPRESSION) {
							printf("ERROR: expected expression at [%d:%d]\n", previous->token.line, previous->token.pos);
							return NULL;
						}
						BinaryExpr *bin_expr = binary_expr_create(&(previous->expr), current->token.data, &(next->expr));
						Expr *expr = binary_expr_to_expr(bin_expr);
						ParserObject *obj = malloc(sizeof(ParserObject));
						obj->type = TYPE_EXPRESSION;
						obj->expr = *expr;
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


    printf("------------------------\n");

    for (int i = 0; i < list->size; i++) {
        ParserObject *current = arraylist_get(list, i);
        if (current->type == TYPE_TOKEN) {
            printf("Token: %s\n", current->token.data);
        } else { // EXPRESSION
            Expr e = current->expr;
            printf("Expression: %d\n", e.type);
            if (current->type != TYPE_EXPRESSION) {
                printf("not an expression: %d\n", current->type);
            }
        }
    }

    Expr *expr = malloc(sizeof(Expr));
    expr->type = TYPE_EXPRESSION;

    arraylist_clear(list);
    free(list);

    return expr;
}
