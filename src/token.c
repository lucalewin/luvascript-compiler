#include <stdio.h>
#include <stdlib.h>

#include "include/token.h"

// Token

Token* token_create(char* data, TokenType type, int pos, int line) {
    Token* t = malloc(sizeof(Token));
    t->data = data;
    t->type = type;
    t->pos = pos;
    t->line = line;
    return t;
}

void token_free(Token* t) {
    free(t->data);
    free(t);
}

// TokenList

TokenList* tokenlist_init() {
    TokenList *list = malloc(sizeof(TokenList*));
    list->heap_size = 1;
    list->size = 0;
    list->tokens = malloc(sizeof(Token));
    return list;
}

void tokenlist_add(TokenList* list, Token* t) {
    while (list->size >= list->heap_size) {
        list->heap_size *= 2;
        list->tokens = (Token*) realloc(list->tokens, sizeof(Token) * list->heap_size);
    }
    list->tokens[list->size++] = *t;
}

void tokenlist_free(TokenList* list) {
    token_free(list->tokens);
    free(list);
}