#ifndef LUVA_PARSER_H
#define LUVA_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "token.h"
#include "expression.h"
#include "statement.h"

// ----------------------------------------------------------

typedef enum _ParserItemType parser_item_t;
typedef struct _ParserItem ParserItem;

// ----------------------------------------------------------

enum _ParserItemType {
    PI_TOKEN,
    PI_EXPR,
};

struct _ParserItem {
    ParserItem *previous;
    parser_item_t type;
    union {
        Token token;
        Expr *expr;
    };
    ParserItem *next;
};

// ----------------------------------------------------------

void parser_create_ast(TokenList *tokens);
Expr *parser_create_expr(TokenList *list);
Expr *evaluate_parenthesis(ParserItem *start, int depth);

#endif//LUVA_PARSER_H