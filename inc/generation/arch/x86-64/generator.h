#ifndef __LUVA_X86_64_GENERATOR_H__
#define __LUVA_X86_64_GENERATOR_H__

#include <parsing/nodes/ast.h>
#include <options.h>

#include <types/table.h>

char *generate_x86_64_assembly(AST *ast, DataTypeTable *dtt, CommandlineOptions *options);

#endif // __LUVA_X86_64_GENERATOR_H__