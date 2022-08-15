#ifndef LUVA_PARSER_H
#define LUVA_PARSER_H

#include <util/arraylist.h>
#include <lexing/token.h>

#include <parsing/nodes/literal.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/ast.h>
#include <parsing/nodes/package.h>

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

/**
 * @brief parses the source file
 * 
 * @param tokens 
 * @return ArrayList* of `struct package_template` 
 */
ArrayList *parse_header(ArrayList *tokens);

Package *parse(ArrayList *tokens, const char *filename);

Function *expectExternFunction();
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
Expression *expectExpressionList();
Expression *expectExpression();
Expression *expectAssignmentExpression();
Expression *expectConditionalExpression();
Expression *expectLogicalOrExpression();
Expression *expectLogicalAndExpression();
Expression *expectBitwiseOrExpression();
Expression *expectBitwiseXorExpression();
Expression *expectBitwiseAndExpression();
Expression *expectEqualitiyExpression();
Expression *expectRelationalExpression();
Expression *expectShiftExpression();
Expression *expectAdditiveExpression();
Expression *expectMultiplicativeExpression();
Expression *expectUnaryExpression();
Expression *expectPostfixExpression();
Expression *expectPrimaryExpression();

// literals
Literal *expectLiteral();

#endif