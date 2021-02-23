#ifndef LUVA_LEXER_H
#define LUVA_LEXER_H

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

TokenList *lexer_start(char *code);

#endif // LUVA_LEXER_H