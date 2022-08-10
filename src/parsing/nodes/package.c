#include <parsing/nodes/package.h>

#include <util/arraylist.h>
#include <util/logging/logger.h>

#include <parsing/scope_impl.h>
#include <parsing/nodes/import.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/enum.h>

Package *package_new() {
	Package *package = malloc(sizeof(Package));
	package->name = NULL;
	package->file_path = NULL;
	package->extern_functions = arraylist_create();
	package->functions = arraylist_create();
	package->global_variables = arraylist_create();
	package->import_declarations = arraylist_create();
	package->imported_functions = arraylist_create();
	package->imported_global_variables = arraylist_create();
	package->package_scope = NULL;
	return package;
}

void package_merge(Package *dest, Package *src) {
	arraylist_addall(dest->functions, src->functions);
	arraylist_addall(dest->global_variables, src->global_variables);
	arraylist_addall(dest->import_declarations, src->import_declarations);
	arraylist_addall(dest->imported_functions, src->imported_functions);
	arraylist_addall(dest->imported_global_variables, src->imported_global_variables);
	scope_merge(dest->package_scope, src->package_scope);
}

void package_free(Package *package) {
	if (package == NULL) {
		return;
	}
	free(package->name);
	free(package->file_path);
	scope_free(package->package_scope);

	for (size_t i = 0; i < package->import_declarations->size; i++) {
		ImportDeclaration *import_declaration = arraylist_get(package->import_declarations, i);
		import_declaration_free(import_declaration);
	}

	if (package->functions != NULL) {
		for (size_t i = 0; i < package->functions->size; i++) {
			Function *function = arraylist_get(package->functions, i);
			function_free(function);
		}
		arraylist_free(package->functions);
	}

	for (size_t i = 0; i < package->extern_functions->size; i++) {
		FunctionTemplate *function_template = arraylist_get(package->extern_functions, i);
		function_template_free(function_template);
	}

	for (size_t i = 0; i < package->global_variables->size; i++) {
		Variable *variable = arraylist_get(package->global_variables, i);
		variable_free(variable);
	}

	// free imported functions and variables
	for (size_t i = 0; i < package->imported_functions->size; i++) {
		FunctionTemplate *function = arraylist_get(package->imported_functions, i);
		function_template_free(function);
	}

	for (size_t i = 0; i < package->imported_global_variables->size; i++) {
		VariableTemplate *variable = arraylist_get(package->imported_global_variables, i);
		variable_template_free(variable);
	}

	// free enum definitions
	for (size_t i = 0; i < package->enum_definitions->size; i++) {
		EnumDefinition *enum_definition = arraylist_get(package->enum_definitions, i);
		enum_definition_free(enum_definition);
	}
}

void package_template_free(struct package_template *package_template)
{
	free(package_template->name);
	arraylist_free(package_template->global_function_identifiers);
	arraylist_free(package_template->global_variable_identifiers);
	arraylist_free(package_template->extern_function_identifiers);
	free(package_template);
}