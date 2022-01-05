#ifndef LUVA_PARSER_H
#define LUVA_PARSER_H

#include <util/arraylist.h>
#include <token.h>

#include <types/literal.h>
#include <types/expression.h>
#include <types/statement.h>
#include <types/function.h>
#include <types/ast.h>
#include <types/package.h>

// helper functions

/**
 * 
 * returns true if the current tokens is an assignment operator
 * 
 * for a list of assignment operators take a look at the documentation at operator precendece 15
 *  at https://github.com/lucr4ft/luvascript-compiler/blob/develop/docs/order_of_opartions.md
 * 
 */
int isAssignmentOperator(Token *t);

/**
 * 
 * returns true if the current tokens is an unary operator
 * 
 * for a list of unary operators take a look at the documentation at operator precendece 2
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
 * same as
 * 
 * expect(token);
 * next();
 * 
 */
void eat(TokenType type);

/**
 * 
 * method to print error message and exit with exit code 1
 * 
 */
void error(const char *msg);

Package *parse(ArrayList *tokens);

FunctionTemplate *expectExternFunctionTemplate();
Function *expectFunction();

// statements
Statement *expectStatement();
Statement *expectCompoundStatement();
Statement *expectExpressionStatement();
Statement *expectJumpStatement();
Statement *expectVariableDeclarationStatement();
Statement *expectConditionalStatement();
Statement *expectLoopStatement();

// expressions
Expression_T *expectExpressionList();
Expression_T *expectExpression();
Expression_T *expectAssignmentExpression();
Expression_T *expectConditionalExpression();
Expression_T *expectLogicalOrExpression();
Expression_T *expectLogicalAndExpression();
Expression_T *expectBitwiseOrExpression();
Expression_T *expectBitwiseXorExpression();
Expression_T *expectBitwiseAndExpression();
Expression_T *expectEqualitiyExpression();
Expression_T *expectRelationalExpression();
Expression_T *expectShiftExpression();
Expression_T *expectAdditiveExpression();
Expression_T *expectMultiplicativeExpression();
Expression_T *expectUnaryExpression();
Expression_T *expectPostfixExpression();
Expression_T *expectPrimaryExpression();

// literals
Literal_T *expectLiteral();

#endif