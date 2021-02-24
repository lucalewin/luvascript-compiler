#ifndef LUVA_PARSER_H
#define LUVA_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "arraylist.h"
#include "token.h"
#include "expression.h"
#include "statement.h"

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
        Token token;
        Expr expr;
        Statement statement;
    };
};

// ----------------------------------------------------------

void parser_create_ast(TokenList *tokens);
Expr *parser_create_expr(TokenList *list);
Expr *evaluate_parenthesis(ArrayList *list);

#endif//LUVA_PARSER_H