#ifndef __LUVASCRIPT_GENERATOR_H__
#define __LUVASCRIPT_GENERATOR_H__

#include <stdio.h>

#include <options.h>
#include <parsing/nodes/ast.h>
#include <types/table.h>

char *generate_assembly(AST *ast, DataTypeTable *dtt, CommandlineOptions *options);
// char *generate_header(AST *ast, CommandlineOptions *options);

#endif // __LUVASCRIPT_GENERATOR_H__