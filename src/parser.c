#include "include/parser.h"

void parser_create_ast(TokenList *tokens) {
    Statement *root = malloc(sizeof(Statement));
    root->expr = parser_create_expr(tokens);
    free(root->expr);
    free(root);
}

// ----------------------------------------------------------------------------------------------------------

Expr *parser_create_expr(TokenList *list) {
    Expr *expr = malloc(sizeof(Expr));

    ParserItem *start = malloc(sizeof(ParserItem));
    ParserItem *current = start;

    int index = 0;

    /* convert literal tokens to literal expressions */
    for (int i = 0; i < list->size; i++, index++) {
        current->type = PI_EXPR; // if not then it will be reset in switch default case
        switch (list->tokens[i].type) {
            case TOKEN_IDENDIFIER: {
                // printf("IDENTIFIER\n");
                LiteralExpr *le = literal_expr_create(IDENTIFIER, list->tokens[i].data);
                current->expr = literal_expr_to_expr(le);
                break;
            }
            case TOKEN_STRING: {
                // printf("STRING\n");
                LiteralExpr *le = literal_expr_create(STRING, list->tokens[i].data);
                current->expr = literal_expr_to_expr(le);
                break;
            }
            case TOKEN_NUMBER: {
                // printf("NUMBER\n");
                LiteralExpr *le = literal_expr_create(NUMBER, list->tokens[i].data);
                current->expr = literal_expr_to_expr(le);
                break;
            }
            default: {
                // printf("TOKEN\n");
                current->token = list->tokens[i];
                current->type = PI_TOKEN;
                break;
            }
        }
        // create next ParserItem
        ParserItem *next = malloc(sizeof(ParserItem));
        next->previous = current;
        current->next = next;
        current = next;
        current->type = -1;
    }

    /* evaluate parenthesis */
    evaluate_parenthesis(start, index);

    // printf("\n\n\nTEST: %d\n", current->index);
    printf("NUMBER: %d\n", current->previous->expr->l->number);
    return expr;
}

// ----------------------------------------------------------------------------------------------------------

Expr *evaluate_parenthesis(ParserItem *start, int depth) {
    ParserItem *current = start;
    printf("------------------------\n");
    printf("evaluation parenthesis\n");
    printf("depth: %d\n", depth);

    for (int i = 0; i < depth; i++) {
        // printf("ITEM type: %d\n", current->type);
        if (current->type == PI_TOKEN) {
            if (current->token.type == TOKEN_LPAREN) {
                ParserItem *paren_begin = current->next;
                int open = 1, close = 0, length = 0;
                do {
                    current = current->next;
                    if (current->type < 0) { // end of list reached and one or more parenthesis are not closed -> print error + return null;
                        printf("\nERROR: Parenthesis at [%d:%d] is never closed!\n", paren_begin->token.line, paren_begin->token.pos);
                        return NULL;
                    }
                    if (current->token.type == TOKEN_LPAREN) open++;
                    else if (current->token.type == TOKEN_RPAREN) close++;
                    length++;
                } while (open > close);
                i += length;
                evaluate_parenthesis(paren_begin, length - 1);
            }
        }
        // next item
        current = current->next;
    }


    /* evaluate *, / and % operators */
    current = start;
    for (int i = 0; i < depth; i++) {
        // printf("ITEM type: %d\n", current->type);
        if (current->type == PI_TOKEN) {
            switch (current->token.type)
            {
                case TOKEN_ASTERISK:
                case TOKEN_SLASH:
                case TOKEN_MOD:
                    {
                        if (current->previous->type != PI_EXPR || current->next->type != PI_EXPR) { // error
                            printf("ERROR: expected expression at [%d:%d]\n", current->previous->token.line, current->previous->token.pos);
                        }
                        BinaryExpr *bin_expr = binary_expr_create(current->previous->expr, current->token.data, current->next->expr);
                        Expr *expr = binary_expr_to_expr(bin_expr);
                        current->previous->type;
                        current->previous->expr = expr;
                        current->next->next->previous = current->previous;
                        current->previous->next = current->next->next;
                        current = current->previous;
                        depth -= 2;
                        i--;
                    }
                    break;
                
                default:
                    break;
            }
        }
        // next item
        current = current->next;
    }

    current = start;
    for (int i = 0; i < depth; i++) {
        printf("ITEM type: %d\n", current->type);
        // next item
        current = current->next;
    }

    printf("------------------------\n");

}