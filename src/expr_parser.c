#include <expr_parser.h>

char* types[] = {
    "expr_expression",
    "expr_assignment",
    "expr_conditional",
    "expr_logicalOr",
    "expr_locicalAnd",
    "expr_bitwiseOr",
    "expr_bitwiseXor",
    "expr_bitwiseAnd",
    "expr_equality",
    "expr_relational",
    "expr_shift",
    "expr_additive",
    "expr_multiplicative",
    "expr_unary",
    "expr_postfix",
    "expr_primary"
};

void exprParserStart(ArrayList* list) {
    tokens = list;
    index = 0;

    // load tokens into 'current' and 'lookahead'
    next();

    // allocate memory for root expression
    expr_node_t *root = createNode();

    // expecting an expression
    // expression(root);
    root = additiveExpr();
    printNode(root);
    
    // free(root);
}

// ------------------------ parsing methods ------------------------

NODE *expressionList() {
}

NODE *expression() {
}

NODE *assignmentExpr() {
}

NODE *conditionalExpr() {
}

NODE *logicalOrExpr() {
}

NODE *locicalAndExpr() {
}

NODE *bitwiseOrExpr() {
}

NODE *bitwiseXorExpr() {
}

NODE *bitwiseAndExpr() {
}

NODE *equalityExpr() {
}

NODE *relationalExpr() {
    NODE *node = createNode();
    node->type = expr_relational;

    exprNodeAdd(node, shiftExpr());

    if (is(TOKEN_RELATIONAL_LESS) || is(TOKEN_RELATIONAL_LESS_OR_EQUAL) ||
                is(TOKEN_RELATIONAL_GREATER) || is(TOKEN_RELATIONAL_GREATER_OR_EQUAL)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, shiftExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_RELATIONAL_LESS) && !is(TOKEN_RELATIONAL_LESS_OR_EQUAL) &&
                    !is(TOKEN_RELATIONAL_GREATER) && !is(TOKEN_RELATIONAL_GREATER_OR_EQUAL)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_RELATIONAL_LESS) || is(TOKEN_RELATIONAL_LESS_OR_EQUAL) ||
                is(TOKEN_RELATIONAL_GREATER) || is(TOKEN_RELATIONAL_GREATER_OR_EQUAL)) {
            node = createNode();
            node->type = expr_relational;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, shiftExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *shiftExpr() {
    // add {('<<' | '>>') shift}
    NODE *node = createNode();
    node->type = expr_shift;

    exprNodeAdd(node, additiveExpr());

    if (is(TOKEN_BITWISE_LEFT_SHIFT) || is(TOKEN_BITWISE_RIGHT_SHIFT)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, additiveExpr());

        // if next token is not '<<' and '>>' return the node
        if (!is(TOKEN_BITWISE_LEFT_SHIFT) && !is(TOKEN_BITWISE_RIGHT_SHIFT)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_BITWISE_LEFT_SHIFT) || is(TOKEN_BITWISE_RIGHT_SHIFT)) {
            node = createNode();
            node->type = expr_shift;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, additiveExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *additiveExpr() {
    NODE *node = createNode();
    node->type = expr_additive;

    // mul
    exprNodeAdd(node, multiplicativeExpr());

    // mul ('+' | '-') mul
    if (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, multiplicativeExpr());

        // if next token is not '+' and '-' return the node
        if (!is(TOKEN_PLUS) && !is(TOKEN_MINUS)) {
            return node;
        }

        // mul {('+' | '-') mul} 
        // if multiple (more than one) additive expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
            node = createNode();
            node->type = expr_additive;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, multiplicativeExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *multiplicativeExpr() {
    NODE *node = createNode();
    node->type = expr_multiplicative;

    // unary
    exprNodeAdd(node, unaryExpr());

    // unary ('*' | '/' | '%') unary
    if (is(TOKEN_ASTERISK) || is(TOKEN_SLASH) || is(TOKEN_MOD)) {
        printf("UJUHKUGU\n");
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, unaryExpr());

        // if next token is not '+' and '-' return the node
        if (!is(TOKEN_ASTERISK) && !is(TOKEN_SLASH) && !is(TOKEN_MOD)) {
            return node;
        }

        // unary {('*' | '/' | '%') unary} 
        // if multiple (more than one) multiplicative expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_ASTERISK) || is(TOKEN_SLASH) || is(TOKEN_MOD)) {
            node = createNode();
            node->type = expr_multiplicative;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, unaryExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *unaryExpr() {
    // NODE *node = createNode();
    // if (isUnaryOperator(current)) {
    //     exprNodeAdd(node, tokenToNode(current));
    //     next();
    //     exprNodeAdd(node, unaryExpr());
    //     return node;
    // }
    return postfixExpr();
}

NODE *postfixExpr() {
    NODE *node = createNode();
    exprNodeAdd(node, primaryExpr());

    if (is(TOKEN_LPAREN)) {
        // function call expression
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, expression());
        expect(TOKEN_RPAREN);
        exprNodeAdd(node, tokenToNode(current));
        next();
    } else if (is(TOKEN_LBRACKET)) {
        // indexing expression
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, expression());
        expect(TOKEN_RBRACKET);
        exprNodeAdd(node, tokenToNode(current));
        next();
    } else if (is(TOKEN_INCREMENT) || is(TOKEN_DECREMENT)) {
        // postfix increment/decrement
        exprNodeAdd(node, tokenToNode(current));
        next();
    }
    return node;
}

NODE *primaryExpr() {
    expr_node_t *primary_node = createNode();
    primary_node->type = expr_primary;
    if (is(TOKEN_LPAREN)) {
        // '(' expression ')'
        exprNodeAdd(primary_node, tokenToNode(current));
        next();
        exprNodeAdd(primary_node, expression(primary_node));
        expect(TOKEN_RPAREN);
        exprNodeAdd(primary_node, tokenToNode(current));
    } else if (is(TOKEN_IDENDIFIER) || is(TOKEN_NUMBER) || is(TOKEN_STRING)) {
        // exprNodeAdd(primary_node, tokenToNode(current));
        free(primary_node);
        primary_node = tokenToNode(current);
        primary_node->type = expr_primary;
    } else {
        error("Expected literal or nested expression!");
    }
    next();
    return primary_node;
}

// ------------------------ utility methods ------------------------

void exprNodeAdd(expr_node_t *parent, expr_node_t *node) {
    parent->childrenCount++;
    parent->children = realloc(parent->children, sizeof(expr_node_t) * parent->childrenCount);
    parent->children[parent->childrenCount - 1] = node;

}

NODE *tokenToNode(Token *t) {
    expr_node_t *node = createNode();
    node->type = token;
    node->value = malloc(strlen(t->data));
    strcpy(node->value, t->data);
    return node;
}

NODE *createNode() {
    NODE *node = malloc(sizeof(NODE));
    node->childrenCount = 0;
    node->children = malloc(sizeof(NODE));
    node->value = NULL;
    node->type = 0;
}

void printNode(NODE *node) {
    if (node == NULL) {
        printf("NULL\n");
        return;
    }
    printf("{\"type\": \"%s\"", types[node->type]);
    if (node->value != NULL) {
        printf(",\"value\": \"%s\"", node->value);
    }
    if (node->childrenCount > 0) {
        printf(",\"children\": [");
    }
    for (int i = 0; i < node->childrenCount; i++) {
        printNode(node->children[i]);
        printf(",");
    }
    if (node->childrenCount > 0) {
        printf("]");
    }
    printf("}");
}

int isAssignmentOperator(Token *t) {
    switch(t->type) {
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
        case TOKEN_ASSIGNMENT_BITWISE_OR:
            return 1;
        default:
            return 0;
    }
}

int isUnaryOperator(Token *t) {
    switch(t->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_TILDE:
        case TOKEN_NOT:
            return 1;
        default:
            return 0;
    }
}

int expect(TokenType type) {
    if (current->type != type) {
        printf("ERROR: Expected %d at [%d,%d]\n", type, current->line, current->pos);
        exit(1);
    }
    return 1;
}

int is(TokenType type) {
    if (current != NULL) {
        return current->type == type;
    } else {
        return 0;
    }
}

void next() {
    current = arraylist_get(tokens, index++);
    if (index < tokens->size) {
        lookahead = arraylist_get(tokens, index);
    } else {
        lookahead = NULL;
    }
}

void error(const char *msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}
