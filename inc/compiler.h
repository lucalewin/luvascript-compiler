#ifndef LUVA_COMPILER_H
#define LUVA_COMPILER_H

#include <types/ast.h>

char *compile_to_x86_64_assembly(AST *ast);
// char *compile_to_arm32_assembly(AST *ast);
// char *compile_to_arm64_assembly(AST *ast);

void compile_asm_file(char *src_file, char *out_file);

#endif