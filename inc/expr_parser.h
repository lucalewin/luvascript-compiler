#ifndef LUVA_EXPR_PARSER_H
#define LUVA_EXPR_PARSER_H

#include <arraylist.h>
#include <token.h>
#include <expression.h>

// --------------------------------

/**
 * 
 * global variables used by the expression parser
 * 
 */
int index;
ArrayList *tokens;
Token *current;
Token *lookahead;

// --------------------------------

/**
 * 
 * struct NODE is used to create a parse tree
 * 
 */
typedef struct expr_node_s expr_node_t;
typedef struct expr_node_s NODE;

struct expr_node_s {
    enum {
        expr_list,
        expr_expression,
        expr_assignment,
        expr_conditional,
        expr_logicalOr,
        expr_locicalAnd,
        expr_bitwiseOr,
        expr_bitwiseXor,
        expr_bitwiseAnd,
        expr_equality,
        expr_relational,
        expr_shift,
        expr_additive,
        expr_multiplicative,
        expr_unary,
        expr_postfix,
        expr_primary,
        token
    } type;
    char *value;
    size_t childrenCount;
    expr_node_t** children;
};

// --------------------------------

void exprParserStart(ArrayList* list);

/**
 * 
 * These methods are used to create a parse tree
 * the method name come from the rules defined in the grammar
 * at (docs)     https://github.com/lucr4ft/luvascript-compiler/blob/develop/docs/grammar.md
 * or (grammar)  https://github.com/lucr4ft/luvascript-compiler/blob/develop/grammar.ebnf
 * 
 */
NODE *function();
NODE *var_decl();

NODE *statement();
NODE *compundStmt();
NODE *blockStmt();
NODE *expressionStmt();
NODE *jumpStmt();

NODE *expressionList();      // expr, expr      expr
NODE *expression();          // any type of expression
NODE *assignmentExpr();      // x = y     x += y
NODE *conditionalExpr();     // ternary
NODE *logicalOrExpr();       // x || y
NODE *locicalAndExpr();      // x && y
NODE *bitwiseOrExpr();       // x | y
NODE *bitwiseXorExpr();      // x ^ y
NODE *bitwiseAndExpr();      // x & y
NODE *equalityExpr();        // x == y    y != x
NODE *relationalExpr();      // x < y     x >= y
NODE *shiftExpr();           // x << y    x >> y
NODE *additiveExpr();        // x + y     x - y
NODE *multiplicativeExpr();  // x * y     x / y
NODE *unaryExpr();           // !x        ++y
NODE *postfixExpr();         // x++       y++
NODE *primaryExpr();         // <identifier> <number> <string>

// --------------------------------

/**
 * 
 * methood to add a (child-)node to a parent node
 * 
 */
void exprNodeAdd(expr_node_t *parent, expr_node_t *node);

/**
 * 
 * utility method to create a new node an copy the token data into node data
 * 
 */
NODE *tokenToNode(Token *t);

/**
 * 
 * utility method to allocate memory for new Node + initialization with default values
 * 
 */
NODE *createNode();

/**
 * 
 * utility method to print type, value and children of a node to console
 * 
 */
void printNode(NODE *node);

/**
 * 
 * returns true if the current tokens is an assignment operator
 * 
 * for a list of assignment operators take a look at the documentation at opertaor precendece 15
 *  at https://github.com/lucr4ft/luvascript-compiler/blob/develop/docs/order_of_opartions.md
 * 
 */
int isAssignmentOperator(Token *t);

/**
 * 
 * returns true if the current tokens is an unary operator
 * 
 * for a list of unary operators take a look at the documentation at opertaor precendece 2
 *  at https://github.com/lucr4ft/luvascript-compiler/blob/develop/docs/order_of_opartions.md
 * 
 */
int isUnaryOperator(Token *t);

/**
 * 
 * checks if the current token is of type 'type'
 * 
 * else an error is thrown and the program exits
 * 
 */
int expect(TokenType type);

/**
 * 
 * checks if the current token is of type 'type'
 * 
 * returns 1 if current->type == type, else 0
 * 
 */
int is(TokenType type);

/**
 * 
 * move token pointer one forward
 * 
 */
void next();

/**
 * 
 * method to print error message and exit with exit code 1
 * 
 */
void error(const char *msg);

#endif