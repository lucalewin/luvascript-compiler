#ifndef __LUVA_MODULE_H__
#define __LUVA_MODULE_H__

#include <util/arraylist.h>

struct module {
	char *name;
	ArrayList *package_templates;
};

struct module *module_load(const char *path);
void module_free(struct module *module);

#endif // __LUVA_MODULE_H__