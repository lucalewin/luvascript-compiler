#ifndef LUVA_PARSER2_H
#define LUVA_PARSER2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util.h>
#include <arraylist.h>
#include <token.h>
#include <debug.h>
#include <ast.h>
#include <type.h>
#include <expression.h>
#include <statement.h>
#include <variable.h>
#include <function.h>

void parse(AST *ast, ArrayList *tokens);

int expect_var_decl(VariableDecl *var_decl);


int expect_function(Function *func);
int expect_func_param_decl(FuncParam *func_param);
int expect_func_return_type(FuncReturnType *func_ret_type);

int expect_statement(Statement *stmt);
int expect_block_statement(BlockStatement *b_stmt);
int expect_return_statement(ReturnStatement *r_stmt);
int expect_expression_statement(ExprStatement *e_stmt);

int expect_expression(Expr *expr);

int expect_datatype(DataType *type);

int expect_token(TokenType expected, Token *actual);

#endif // LUVA_PARSER2_H