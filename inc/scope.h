#ifndef LUVA_SCOPE_H
#define LUVA_SCOPE_H

#include <util/arraylist.h>

typedef struct _scope {
	ArrayList *variables;
	ArrayList *functions;
	ArrayList *function_templates;
} Scope;

#endif