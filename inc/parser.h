#ifndef LUVA_PARSER_H
#define LUVA_PARSER_H

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
#include <function.h>

// ----------------------------------------------------------

typedef enum _ParserObjectType parser_object_t;
typedef struct _ParserObject ParserObject;

// ----------------------------------------------------------

enum _ParserObjectType {
    TYPE_TOKEN,
    TYPE_EXPRESSION,
    TYPE_STATEMENT
};

struct _ParserObject {
    parser_object_t type;
    union {
        Token *token;
        Expr *expr;
        Statement *statement;
    };
};

// ----------------------------------------------------------

AST *parser_create_ast(ArrayList *tokens);

Function *parse_function(ArrayList *tokens);

ArrayList *parse_func_params_decl(ArrayList *tokens);
FuncParam *parse_func_param_decl(ArrayList *tokens);

ArrayList *parse_func_return_types(ArrayList *tokens);
FuncReturnType *parse_func_return_type(ArrayList* tokens);

Statement *parse_statement(ArrayList *tokens);
BlockStatement *parse_block_statement(ArrayList *tokens);

DataType *parse_data_type(Token *token);

Expr *parse_expression(ArrayList *tokens);
Expr *parser_create_expr(ArrayList *list);
Expr *evaluate_parenthesis(ArrayList *list);

// ----------------------------------------------------------

char *convert_ast_to_x86_64_assembly(AST *ast);
char *convert_ast_to_arm64_assembly(AST *ast);

#endif // LUVA_PARSER_H