#include <x86-64/generator.h>

#include <x86-64/compiler.h>

char *generate_x86_64_assembly(AST *ast, CommandlineOptions *options) {
	// TODO
	return compile_to_x86_64_assembly(ast, options);
}