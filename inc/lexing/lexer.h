#ifndef LUVA_LEXER_H
#define LUVA_LEXER_H

#include <util/arraylist.h>

/**
 * @author Luca Lewin
 * @brief takes the source code as input and returns a list of tokens
 * 
 * @param code the content of the source file
 * @return ArrayList* of Token*
 */
ArrayList *tokenize(char *code, const char *file);

#endif // LUVA_LEXER_H