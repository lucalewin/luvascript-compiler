#ifndef LUVA_LEXER_H
#define LUVA_LEXER_H

#include <stdio.h>
#include <stdlib.h>

#include "arraylist.h"
#include "util.h"
#include "token.h"

ArrayList *lexer_start(char *code);

#endif // LUVA_LEXER_H