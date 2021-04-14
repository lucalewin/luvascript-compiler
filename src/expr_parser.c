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

NODE *expression(expr_node_t *node) {
    expr_node_t *assignmentExprNode = malloc(sizeof(expr_node_t));
    assignmentExprNode->type = expr_assignment;
    assignmentExpr(assignmentExprNode);
    exprNodeAdd(node, assignmentExprNode);
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
}

NODE *shiftExpr() {
    // add {('<<' | '>>') shift}
    NODE *node = createNode();
    node->type = expr_shift;
}

NODE *additiveExpr() {
    // mul {('+' | '-') mul}

    NODE *node = createNode();
    node->type = expr_additive;

    exprNodeAdd(node, multiplicativeExpr());
    if (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, multiplicativeExpr());

        // if next token is not '+' and '-' return the node
        if (!is(TOKEN_PLUS) && !is(TOKEN_MINUS)) {
            return node;
        }

        // if multiple (more than one) additive expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        NODE *top;
        while (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
            top = createNode();
            top->type = expr_additive;
            exprNodeAdd(top, temp);
            exprNodeAdd(top, tokenToNode(current));
            next();
            exprNodeAdd(top, multiplicativeExpr());
            temp = top;
        }
        // only free pointer, not it's value
        // temp = NULL;
        // free(temp);
        return top;
    }
}

NODE *multiplicativeExpr() {
    // unary {('*' | '/' | '%') unary}
    
    return unaryExpr();
}

NODE *unaryExpr() {
    return postfixExpr();
}

NODE *postfixExpr() {
    return primaryExpr();
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
    printf("{type: %s", types[node->type]);
    if (node->value != NULL) {
        printf(",value: %s", node->value);
    }
    if (node->childrenCount > 0) {
        printf(",\"children\": [");
    }
    for (int i = 0; i < node->childrenCount; i++) {
        printf(",");
        printNode(node->children[i]);
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
