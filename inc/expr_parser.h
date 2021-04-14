#pragma once

#ifndef LUVA_EXPR_PARSER_H
#define LUVA_EXPR_PARSER_H

#include <arraylist.h>
#include <token.h>
#include <expression.h>

// --------------------------------

int index;
ArrayList *tokens;
Token *current;
Token *lookahead;

// --------------------------------

typedef struct expr_node_s expr_node_t;
typedef struct expr_node_s NODE;

struct expr_node_s {
    enum {
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

NODE *expression();
NODE *assignmentExpr();
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
 * 
 * 
 */
void exprNodeAdd(expr_node_t *parent, expr_node_t *node);

/**
 * 
 * 
 * 
 */
NODE *tokenToNode(Token *t);

/**
 * 
 * 
 * 
 */
NODE *createNode();

/**
 * 
 * 
 * 
 */
void printNode(NODE *node);

/**
 * 
 * 
 * 
 */
int isAssignmentOperator(Token *t);

/**
 * 
 * 
 * 
 */
int isUnaryOperator(Token *t);

/**
 * 
 * 
 * 
 */
int expect(TokenType type);

/**
 * 
 * 
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