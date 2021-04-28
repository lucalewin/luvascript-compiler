#include <parser.h>

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

NODE *program();

NODE *test();

void exprParserStart(ArrayList* list) {

    tokens = list;
    index = 0;

    // load tokens into 'current' and 'lookahead'
    next();

    // parse root expression
    expr_node_t *root = program(); //expression();

    // print root expression
    printNode(root);

    // free root expression
    free(root);
}

// ------------------------ parsing methods ------------------------

NODE *program() {
    NODE *root = createNode();

    while(index < tokens->size) {
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
    }

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

    // optional: param delaration
    if (!is(TOKEN_RPAREN)) {
        // TODO: implement param declaration
        exprNodeAdd(node, funcParamDeclList());
    }

    // expecting rparen
    expect(TOKEN_RPAREN);
    exprNodeAdd(node, tokenToNode(current));
    next();

    // optional: return type declaration
    if (is(TOKEN_COLON)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, funcReturnTypeDeclList());
    }

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

NODE *funcParamDeclList() {
    NODE *node = createNode();

    exprNodeAdd(node, funcParamDecl());

    if (is(TOKEN_COMMA)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, funcParamDecl());

        if (!is(TOKEN_COMMA)) {
            return node;
        }

        NODE *temp = node;
        while (is(TOKEN_COMMA)) {
            node = createNode();
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, funcParamDecl());
            temp = node;
        }
        temp = NULL;
        free(temp);
        return node;
    }
    return node;

    return node;
}

NODE *funcParamDecl() {
    NODE *node = createNode();

    expect(TOKEN_IDENDIFIER);
    exprNodeAdd(node, tokenToNode(current));
    next();

    expect(TOKEN_COLON);
    exprNodeAdd(node, tokenToNode(current));
    next();

    if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
        error("Expected type name");
    }
    // TODO: replace token with type
    // -> exprNodeAdd(node, type());
    exprNodeAdd(node, tokenToNode(current));
    next();

    // check if default value is assigned
    if (is(TOKEN_ASSIGNMENT_SIMPLE)) {
        // expecting default value assignment expression
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, expression());
    }

    return node;
}

NODE *funcReturnTypeDeclList() {
    NODE *node = createNode();

    exprNodeAdd(node, funcReturnTypeDecl());

    if (is(TOKEN_COMMA)) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, funcReturnTypeDecl());

        if (!is(TOKEN_COMMA)) {
            return node;
        }

        NODE *temp = node;
        while (is(TOKEN_COMMA)) {
            node = createNode();
            exprNodeAdd(node, temp);
            exprNodeAdd(node, tokenToNode(current));
            next();
            exprNodeAdd(node, funcReturnTypeDecl());
            temp = node;
        }
        temp = NULL;
        free(temp);
        return node;
    }
    return node;
}

NODE *funcReturnTypeDecl() {
    if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
        error("Expected type name");
    }
    NODE *node = createNode();

    // TODO: replace token with type
    // -> exprNodeAdd(node, type());
    exprNodeAdd(node, tokenToNode(current));
    next();

    // check if default value is assigned
    if (is(TOKEN_ASSIGNMENT_SIMPLE)) {
        // expecting default value assignment expression
        exprNodeAdd(node, tokenToNode(current));
        next();
        exprNodeAdd(node, expression());
    }

    return node;
}

// -------------------------- statements ---------------------------

NODE *statement() {
    NODE *node = createNode();

    if (is(TOKEN_LBRACE)) {
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
    NODE *node = createNode();
    if (strcmp(current->data, "return") == 0) {
        exprNodeAdd(node, tokenToNode(current));
        next();
        if (!is(TOKEN_SEMICOLON)) {
            exprNodeAdd(node, expressionList());
        }
        // add semicolon
        exprNodeAdd(node, tokenToNode(current));
        next();
    } else {
        error("expected 'return' keyword!");
    }
    return node;
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
    if (lookahead == NULL || (lookahead->type != TOKEN_LOGICAL_OR)) {
        return locicalAndExpr();
    }

    NODE *node = createNode();
    node->type = expr_logicalOr;
    exprNodeAdd(node, locicalAndExpr());
    NODE *temp = node;

    while(is(TOKEN_LOGICAL_OR)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, locicalAndExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_logicalOr;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *locicalAndExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_LOGICAL_AND)) {
        return bitwiseOrExpr();
    }

    NODE *node = createNode();
    node->type = expr_locicalAnd;
    exprNodeAdd(node, bitwiseOrExpr());
    NODE *temp = node;

    while(is(TOKEN_LOGICAL_AND)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, bitwiseOrExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_locicalAnd;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *bitwiseOrExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_VERTICAL_BAR)) {
        return bitwiseXorExpr();
    }

    NODE *node = createNode();
    node->type = expr_bitwiseOr;
    exprNodeAdd(node, bitwiseXorExpr());
    NODE *temp = node;

    while(is(TOKEN_VERTICAL_BAR)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, bitwiseXorExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_bitwiseOr;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *bitwiseXorExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_CIRCUMFLEX)) {
        return bitwiseAndExpr();
    }

    NODE *node = createNode();
    node->type = expr_bitwiseXor;
    exprNodeAdd(node, bitwiseAndExpr());
    NODE *temp = node;

    while(is(TOKEN_CIRCUMFLEX)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, bitwiseAndExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_bitwiseXor;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *bitwiseAndExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_AMPERSAND)) {
        return equalityExpr();
    }

    NODE *node = createNode();
    node->type = expr_bitwiseAnd;
    exprNodeAdd(node, equalityExpr());
    NODE *temp = node;

    while(is(TOKEN_AMPERSAND)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, equalityExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_bitwiseAnd;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *equalityExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_RELATIONAL_EQUAL && lookahead->type != TOKEN_RELATIONAL_NOT_EQUAL)) {
        return relationalExpr();
    }

    NODE *node = createNode();
    node->type = expr_equality;
    exprNodeAdd(node, relationalExpr());
    NODE *temp = node;

    while(is(TOKEN_RELATIONAL_EQUAL) || is(TOKEN_RELATIONAL_NOT_EQUAL)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, relationalExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_equality;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node; 
}

NODE *relationalExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_RELATIONAL_LESS && lookahead->type != TOKEN_RELATIONAL_LESS_OR_EQUAL && 
                lookahead->type != TOKEN_RELATIONAL_GREATER && lookahead->type != TOKEN_RELATIONAL_GREATER_OR_EQUAL)) {
        return shiftExpr();
    }

    NODE *node = createNode();
    node->type = expr_relational;
    exprNodeAdd(node, shiftExpr());
    NODE *temp = node;

    while(is(TOKEN_RELATIONAL_LESS) || is(TOKEN_RELATIONAL_LESS_OR_EQUAL) || is(TOKEN_RELATIONAL_GREATER) || is(TOKEN_RELATIONAL_GREATER_OR_EQUAL)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, shiftExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_relational;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *shiftExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_BITWISE_LEFT_SHIFT && lookahead->type != TOKEN_BITWISE_RIGHT_SHIFT)) {
        return additiveExpr();
    }

    NODE *node = createNode();
    node->type = expr_shift;
    exprNodeAdd(node, additiveExpr());
    NODE *temp = node;

    while(is(TOKEN_BITWISE_LEFT_SHIFT) || is(TOKEN_BITWISE_RIGHT_SHIFT)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, additiveExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_shift;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *additiveExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_PLUS && lookahead->type != TOKEN_MINUS)) {
        // to simplify the parsetree
        return multiplicativeExpr();
    }

    NODE *node = createNode();
    node->type = expr_additive;
    exprNodeAdd(node, multiplicativeExpr());
    NODE *temp = node;

    while(is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, multiplicativeExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_additive;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);

    return node;
}

NODE *multiplicativeExpr() {
    if (lookahead == NULL || (lookahead->type != TOKEN_ASTERISK && lookahead->type != TOKEN_SLASH && lookahead->type != TOKEN_MOD)) {
        return unaryExpr();
    }

    NODE *node = createNode();
    node->type = expr_multiplicative;
    exprNodeAdd(node, unaryExpr());
    NODE *temp = node;

    while(is(TOKEN_ASTERISK) || is(TOKEN_SLASH) || is(TOKEN_MOD)) {
        exprNodeAdd(temp, tokenToNode(current));
        next();
        exprNodeAdd(temp, unaryExpr());
        node = temp;
        temp = createNode();
        temp->type = expr_multiplicative;
        exprNodeAdd(temp, node);
    }
    temp = NULL;
    free(temp);
    
    return node;
}

NODE *unaryExpr() {
    // TODO: implement unary expression parsing
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
    return node;
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
