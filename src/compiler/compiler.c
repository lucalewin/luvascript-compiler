#include <compiler.h>

#include <x86-64/compiler.h>

#include <options.h>
#include <types/ast.h>

char *compile(AST *ast, CommandlineOptions *options) {
	return compile_to_x86_64_assembly(ast, options);
}