#ifndef __LUVA_PACKAGE_H__
#define __LUVA_PACKAGE_H__

#include <scope.h>
#include <util/arraylist.h>

typedef struct package Package;

struct package {
	char *name;

	// ArrayList *imported_packages;

	ArrayList *import_stmts;

	Scope *package_scope;
	ArrayList *functions;
	ArrayList *global_variables;
	ArrayList *extern_functions;

	ArrayList *imported_functions;
	ArrayList *imported_global_variables;
};

void package_free(Package *package);
void package_merge(Package *dest, Package *src);

#endif // __LUVA_PACKAGE_H__