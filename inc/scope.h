#ifndef LUVA_SCOPE_H
#define LUVA_SCOPE_H

#include <util/arraylist.h>
#include <types/variable.h>

typedef struct _scope {
	ArrayList *variables;
} Scope;

/**
 * @brief create/allocate memory for a new Scope pointer
 * 
 * @return Scope* the newly allocated Scope pointer
 */
Scope *scope_new();

/**
 * @brief deep copy a scope struct
 * 
 * @param scope the scope to be copied
 * @return Scope* the new scope
 */
Scope *scope_copy(Scope *scope);

/**
 * @brief check if the scope contains the variable
 * 
 * @param scope the scope
 * @param name the name of the variable
 * @return int 1 (true) if the variable is in the scope, otherwise 0 (false)
 */
int scope_contains_variable_name(Scope *scope, char *name);

/**
 * @brief add a variable to the scope
 * 
 * @param scope the scope
 * @param var the variable
 */
void scope_add_variable_name(Scope *scope, Variable *var);

/**
 * @brief get the base pointer offset on the stack where the var is located
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int the offset from the base pointer
 */
int scope_get_variable_rbp_offset(Scope *scope, char *var_name);

/**
 * @brief get the variable int the scope with the specified name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return Variable* the pointer of the variable
 */
Variable *scope_get_variable_by_name(Scope *scope, char *var_name);

#endif