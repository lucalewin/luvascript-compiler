#ifndef LUVA_COMPILER_H
#define LUVA_COMPILER_H

// #include <types/ast.h>
#include <options.h>

// char *compile(AST *ast, CommandlineOptions *options);
void compile(const char *filename, CommandlineOptions *options);

#endif