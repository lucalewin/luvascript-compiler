#include <generation/generator.h>

#include <util/util.h>
#include <util/io/file.h>

#include <util/logging/logger.h>

#include <parsing/nodes/function.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/ast.h>

#include <generation/arch/x86-64/generator.h>

char *generate_assembly(AST *ast, DataTypeTable *dtt, CommandlineOptions *options) {
	char *assembly = generate_x86_64_assembly(ast, dtt, options);
	return assembly;
}

// char *generate_header(AST *ast, CommandlineOptions *options) {
// 	char *header_code = "";

// 	// add packages to the header code
// 	for (size_t i = 0; i < ast->packages->size; i++) {

// 		Package *package = arraylist_get(ast->packages, i);
// 		// log_debug("generating package header for package %s\n", package->name);
// 		header_code = straddall(header_code, "package ", package->name, "\n", NULL);

// 		// add external functions to the header code
// 		for (size_t j = 0; j < package->extern_functions->size; j++) {
// 			FunctionTemplate *function = arraylist_get(package->extern_functions, j);
// 			header_code = straddall(header_code, "extern function ", function->identifier, ";\n", NULL);
// 		}

// 		// add functions to the header code
// 		for (size_t j = 0; j < package->functions->size; j++) {
// 			Function *function = arraylist_get(package->functions, j);
// 			// log_debug("generating function header for function %s\n", function->identifier);
// 			header_code = straddall(header_code, "function ", function->identifier, "();\n", NULL);
// 		}

// 		// add global variables to the header code
// 		for (size_t j = 0; j < package->global_variables->size; j++) {
// 			Variable *variable = arraylist_get(package->global_variables, j);
// 			header_code = straddall(
// 					header_code,
// 					variable->is_constant ? "const " : "var ",
// 					variable->identifier, ":", variable->type->type_identifier, ";\n", NULL);
// 		}
// 	}

// 	return header_code;
// }