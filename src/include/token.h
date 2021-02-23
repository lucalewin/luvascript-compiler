#ifndef LUVA_TOKEN_H
#define LUVA_TOKEN_H

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
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_DOT
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

Token* token_create(char* data, TokenType type, int pos, int line);
void token_free(Token* t);

// ----------------------------------------------------------------

struct _TokenList {
    int heap_size;
    int size;
    Token *tokens;
};
typedef struct _TokenList TokenList;

TokenList* tokenlist_init();
void tokenlist_add(TokenList* list, Token* t);
void tokenlist_free(TokenList* list);

// ----------------------------------------------------------------

#endif//LUVA_TOKEN_H