#include <token.h>

// -----------------------------------------------------------------

Token *token_create(char *data, TokenType type, int line, int pos) {
    Token *t = malloc(sizeof(Token));
    t->data = data;
    t->type = type;
    t->pos = pos;
    t->line = line;
    return t;
}

// -----------------------------------------------------------------

void token_free(Token *t) {
    free(t->data);
    free(t);
}
