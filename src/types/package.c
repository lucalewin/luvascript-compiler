#include <types/package.h>

#include <scope_impl.h>
#include <util/arraylist.h>
#include <logging/logger.h>

Package *package_new() {
	Package *package = malloc(sizeof(Package));
	package->extern_functions = arraylist_create();
	package->functions = arraylist_create();
	package->global_variables = arraylist_create();
	package->import_stmts = arraylist_create();
	package->imported_functions = arraylist_create();
	package->imported_global_variables = arraylist_create();
	// package->package_scope = scope_new();
	return package;
}

void package_merge(Package *dest, Package *src) {
	arraylist_addall(dest->functions, src->functions);
	arraylist_addall(dest->global_variables, src->global_variables);
	arraylist_addall(dest->import_stmts, src->import_stmts);
	arraylist_addall(dest->imported_functions, src->imported_functions);
	arraylist_addall(dest->imported_global_variables, src->imported_global_variables);
	scope_merge(dest->package_scope, src->package_scope);
}

void package_template_free(struct package_template *package_template)
{
	free(package_template->name);
	arraylist_free(package_template->global_function_identifiers);
	arraylist_free(package_template->global_variable_identifiers);
	arraylist_free(package_template->extern_function_identifiers);
	free(package_template);
}