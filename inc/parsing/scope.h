#ifndef LUVA_SCOPE_H
#define LUVA_SCOPE_H

#include <util/arraylist.h>

typedef struct _scope Scope;

struct _scope {
	ArrayList *enum_definitions;
	ArrayList *global_variables;
	ArrayList *local_variables;
	ArrayList *functions;
	Scope *parent;
};

#endif