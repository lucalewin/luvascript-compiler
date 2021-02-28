#ifndef LUVA_TOKEN_H
#define LUVA_TOKEN_H

// ----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------

enum _TokenType {
    TOKEN_KEYWORD,
    TOKEN_IDENDIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,
    TOKEN_MOD,
    TOKEN_AMPERSAND,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_DOT,
    TOKEN_POWER,

    /* increment/decrement tokens */
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,

    /* bitwise left/right shift */
    TOKEN_BITWISE_LEFT_SHIFT,
    TOKEN_BITWISE_RIGHT_SHIFT,

    /* relational tokens */
    TOKEN_RELATIONAL_LESS,
    TOKEN_RELATIONAL_LESS_OR_EQUAL,
    TOKEN_RELATIONAL_GREATER,
    TOKEN_RELATIONAL_GREATER_OR_EQUAL,
    TOKEN_RELATIONAL_EQUAL,
    TOKEN_RELATIONAL_NOT_EQUAL,

    /* assignment tokens */
    TOKEN_ASSIGNMENT_SIMPLE,
    TOKEN_ASSIGNMENT_SUM,
    TOKEN_ASSIGNMENT_DIFFERENCE,
    TOKEN_ASSIGNMENT_PRODUCT,
    TOKEN_ASSIGNMENT_QUOTIENT,
    TOKEN_ASSIGNMENT_REMAINDER,
    TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT,
    TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT,
    TOKEN_ASSIGNMENT_BITWISE_AND,
    TOKEN_ASSIGNMENT_BITWISE_XOR,
    TOKEN_ASSIGNMENT_BITWISE_OR,
};
typedef enum _TokenType TokenType;

// ----------------------------------------------------------------

struct _Token {
    char* data;
    TokenType type;
    int pos;
    int line;
};
typedef struct _Token Token;

Token* token_create(char* data, TokenType type, int line, int pos);
void token_free(Token* t);

#endif // LUVA_TOKEN_H