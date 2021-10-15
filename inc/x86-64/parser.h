#ifndef LUVA_PARSER_X86_64_H
#define LUVA_PARSER_X86_64_H

#include <ast.h>

char *parse_ast_to_x86_64_asm(AST *root);

#endif