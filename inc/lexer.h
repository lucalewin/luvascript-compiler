#ifndef LUVA_LEXER_H
#define LUVA_LEXER_H

// ----------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include <arraylist.h>
#include <util.h>
#include <token.h>
#include <assembly.h>

// ----------------------------------------------

// main method prototype to lex the code
ArrayList *lexer_start(char *code);

// prototype methods for lexing inline x86-64 assembly
Assembly *lexer_parse_x86_64(char *code);

// ----------------------------------------------

#endif // LUVA_LEXER_H