#ifndef __LUVA_PACKAGE_H__
#define __LUVA_PACKAGE_H__

#include <scope.h>
#include <util/arraylist.h>

typedef struct package Package;

struct package {
	char *name;

	char *file_path;
	// ArrayList *imported_packages;

	ArrayList *import_declarations;

	Scope *package_scope;
	ArrayList *functions;
	ArrayList *global_variables;
	ArrayList *extern_functions;

	ArrayList *imported_functions;
	ArrayList *imported_global_variables;
};

struct package_template {
	char *name;
	ArrayList *global_function_identifiers;
	ArrayList *global_variable_identifiers;
	ArrayList *extern_function_identifiers;
};

void package_free(Package *package);
void package_merge(Package *dest, Package *src);

void package_template_free(struct package_template *package_template);

#endif // __LUVA_PACKAGE_H__