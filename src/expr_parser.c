#include <expr_parser.h>

char *types[] = {
    "expr_list",
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

NODE *test();

void exprParserStart(ArrayList* list) {

    tokens = list;
    index = 0;

    // load tokens into 'current' and 'lookahead'
    next();

    // parse root expression
    expr_node_t *root = test(); //expression();

    // print root expression
    printNode(root);

    // free root expression
    free(root);
}

// ------------------------ parsing methods ------------------------



NODE *test() {
    NODE *root = createNode();
    NODE *node;

    expect(TOKEN_KEYWORD);

    if (strcmp(current->data, "function") == 0) {
        // function declaration
        node = function();
    } else if (strcmp(current->data, "var") == 0 || strcmp(current->data, "const") == 0) {
        // variable declaration
        node = var_decl();
    } else {
        error("expeced function or variable declaration");
    }
    exprNodeAdd(root, node);

    return root;
}

NODE *function() {
    NODE *node = createNode();

    // adding function keyword to root node
    exprNodeAdd(node, tokenToNode(current));
    next();

    // expecting function identifier
    expect(TOKEN_IDENDIFIER);
    exprNodeAdd(node, tokenToNode(current));
    next();

    // expecting lparen
    expect(TOKEN_LPAREN);
    exprNodeAdd(node, tokenToNode(current));
    next();

    // TODO: implement param declartion

    // expecting rparen
    expect(TOKEN_RPAREN);
    exprNodeAdd(node, tokenToNode(current));
    next();

    // TODO: implement return type declaration

    // expecting block statement
    exprNodeAdd(node, blockStmt());

    return node;
}

NODE *var_decl() {
    NODE *node = createNode();

    // 'var' or 'const'
    exprNodeAdd(node, tokenToNode(current));
    next();

    expect(TOKEN_IDENDIFIER);

    // TODO: implement variable declaration parsing
}

NODE *statement() {
    NODE *node = createNode();

    if (is(TOKEN_RBRACE)) {
        exprNodeAdd(node, blockStmt());
    } else if (is(TOKEN_KEYWORD)) {
        exprNodeAdd(node, jumpStmt());
    } else {
        exprNodeAdd(node, expressionStmt());
    }

    return node;
}

NODE *blockStmt() {
    NODE *node = createNode();

    expect(TOKEN_LBRACE);
    exprNodeAdd(node, tokenToNode(current));
    next();

    while(!is(TOKEN_RBRACE)) {
        exprNodeAdd(node, statement());
    }
    expect(TOKEN_RBRACE);
    exprNodeAdd(node, tokenToNode(current));
    next();

    return node;
}

NODE *expressionStmt() {
    NODE *node = createNode();

    exprNodeAdd(node, expression());

    return node;
}

NODE *jumpStmt() {
    // TODO: implement parsing of jump statements
}

// -------------------------- expressions --------------------------

NODE *expressionList() {
    NODE *node = createNode();
    node->type = expr_list;

    exprNodeAdd(node, expression());

    if (is(TOKEN_COMMA)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, expression());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_COMMA)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_COMMA)) {
            node = createNode();
            node->type = expr_list;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, expression());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *expression() {
    NODE *node = createNode();
    node->type = expr_expression;
    exprNodeAdd(node, assignmentExpr());
    return node;
}

NODE *assignmentExpr() {
    NODE *node = createNode();
    node->type = expr_assignment;
    if (isAssignmentOperator(lookahead)) {
        exprNodeAdd(node, unaryExpr());
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, assignmentExpr());
    } else {
        exprNodeAdd(node, conditionalExpr());
    }
    return node;
}

NODE *conditionalExpr() {
    NODE *node = createNode();
    node->type = expr_conditional;

    exprNodeAdd(node, logicalOrExpr());

    if (is(TOKEN_QUESTION_MARK)) {
        exprNodeAdd(node, tokenToNode(current));
        next();

        exprNodeAdd(node, expression());

        expect(TOKEN_COLON);
        exprNodeAdd(node, tokenToNode(current));
        next();

        exprNodeAdd(node, expression());
    }

    return node;
}

NODE *logicalOrExpr() {
    NODE *node = createNode();
    node->type = expr_logicalOr;

    exprNodeAdd(node, locicalAndExpr());

    if (is(TOKEN_LOGICAL_OR)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, locicalAndExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_LOGICAL_OR)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_LOGICAL_OR)) {
            node = createNode();
            node->type = expr_logicalOr;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, locicalAndExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *locicalAndExpr() {
    NODE *node = createNode();
    node->type = expr_locicalAnd;

    exprNodeAdd(node, bitwiseOrExpr());

    if (is(TOKEN_LOGICAL_AND)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, bitwiseOrExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_LOGICAL_AND)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_LOGICAL_AND)) {
            node = createNode();
            node->type = expr_locicalAnd;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, bitwiseOrExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *bitwiseOrExpr() {
    NODE *node = createNode();
    node->type = expr_bitwiseOr;

    exprNodeAdd(node, bitwiseXorExpr());

    if (is(TOKEN_VERTICAL_BAR)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, bitwiseXorExpr());

        // if next token is not '|' return the node
        if (!is(TOKEN_VERTICAL_BAR)) {
            return node;
        }

        // if multiple (more than one) $ expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_VERTICAL_BAR)) {
            node = createNode();
            node->type = expr_bitwiseOr;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, bitwiseXorExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *bitwiseXorExpr() {
    NODE *node = createNode();
    node->type = expr_bitwiseXor;

    exprNodeAdd(node, bitwiseAndExpr());

    if (is(TOKEN_CIRCUMFLEX)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, bitwiseAndExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_CIRCUMFLEX)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_CIRCUMFLEX)) {
            node = createNode();
            node->type = expr_bitwiseXor;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, bitwiseAndExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *bitwiseAndExpr() {
    NODE *node = createNode();
    node->type = expr_bitwiseAnd;

    exprNodeAdd(node, equalityExpr());

    if (is(TOKEN_AMPERSAND)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, equalityExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_AMPERSAND)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_AMPERSAND)) {
            node = createNode();
            node->type = expr_bitwiseAnd;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, equalityExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *equalityExpr() {
    NODE *node = createNode();
    node->type = expr_equality;

    exprNodeAdd(node, relationalExpr());

    if (is(TOKEN_RELATIONAL_EQUAL) || is(TOKEN_RELATIONAL_NOT_EQUAL)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, relationalExpr());

        // if next token is not '<', '<=', '>' and '>=' return the node
        if (!is(TOKEN_RELATIONAL_EQUAL) && !is(TOKEN_RELATIONAL_NOT_EQUAL)) {
            return node;
        }

        // if multiple (more than one) shift expression are in a row
        // parse them with a while loop and create parse tree
        NODE *temp = node;
        while (is(TOKEN_RELATIONAL_EQUAL) || is(TOKEN_RELATIONAL_NOT_EQUAL)) {
            node = createNode();
            node->type = expr_equality;
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, relationalExpr());
            temp = node;
        }
        // only free pointer, not it's value
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
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
    node->type = expr_postfix;
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
        if (i +1 < node->childrenCount) {
            printf(",");
        }
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

char *token_type_names[] = {
    "TOKEN_KEYWORD",
    "TOKEN_IDENDIFIER",
    "TOKEN_NUMBER",
    "TOKEN_STRING",
    "TOKEN_PLUS",
    "TOKEN_MINUS",
    "TOKEN_ASTERISK",
    "TOKEN_SLASH",
    "TOKEN_MOD",
    "TOKEN_AMPERSAND",
    "TOKEN_VERTICAL_BAR",
    "TOKEN_POWER",
    "TOKEN_CIRCUMFLEX",
    "TOKEN_COLON",
    "TOKEN_SEMICOLON",
    "TOKEN_DOT",
    "TOKEN_COMMA",
    "TOKEN_LPAREN",
    "TOKEN_RPAREN",
    "TOKEN_LBRACKET",
    "TOKEN_RBRACKET",
    "TOKEN_LBRACE",
    "TOKEN_RBRACE",
    "TOKEN_INCREMENT",
    "TOKEN_DECREMENT",
    "TOKEN_BITWISE_LEFT_SHIFT",
    "TOKEN_BITWISE_RIGHT_SHIFT",
    "TOKEN_RELATIONAL_LESS",
    "TOKEN_RELATIONAL_LESS_OR_EQUAL",
    "TOKEN_RELATIONAL_GREATER",
    "TOKEN_RELATIONAL_GREATER_OR_EQUAL",
    "TOKEN_RELATIONAL_EQUAL",
    "TOKEN_RELATIONAL_NOT_EQUAL",
    "TOKEN_LOGICAL_AND",
    "TOKEN_LOGICAL_OR",
    "TOKEN_TILDE",
    "TOKEN_NOT",
    "TOKEN_QUESTION_MARK",
    "TOKEN_ASSIGNMENT_SIMPLE",
    "TOKEN_ASSIGNMENT_SUM",
    "TOKEN_ASSIGNMENT_DIFFERENCE",
    "TOKEN_ASSIGNMENT_PRODUCT",
    "TOKEN_ASSIGNMENT_QUOTIENT",
    "TOKEN_ASSIGNMENT_REMAINDER",
    "TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT",
    "TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT",
    "TOKEN_ASSIGNMENT_BITWISE_AND",
    "TOKEN_ASSIGNMENT_BITWISE_XOR",
    "TOKEN_ASSIGNMENT_BITWISE_OR"
};

int expect(TokenType type) {
    if (current->type != type) {
        printf("ERROR: Expected %s at [%d:%d]\n", token_type_names[type], current->line, current->pos);
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
