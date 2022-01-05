#ifndef __LUVA_X86_64_COMPILER_H__
#define __LUVA_X86_64_COMPILER_H__

#include <options.h>
#include <types/ast.h>

char *compile_to_x86_64_assembly(AST *ast, CommandlineOptions *options);

#endif // __LUVA_X86_64_COMPILER_H__