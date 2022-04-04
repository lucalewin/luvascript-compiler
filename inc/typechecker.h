#ifndef __LUVASCRIPT_TYPECHECKER_H__
#define __LUVASCRIPT_TYPECHECKER_H__

#include <stdbool.h>

#include <types/ast.h>

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool check_types(AST *ast);

/**
 * @brief checks if the given ast contains duplicate functions
 * 
 * @param ast the current ast
 * @return true if duplicate functions were found, false otherwise
 */
bool check_for_duplicate_functions(AST *ast);

/**
 * @brief checks if the given ast contains duplicate variables
 * 
 * @param ast the current ast
 * @return true if duplicate variables were found, false otherwise
 */
bool check_for_duplicate_variables(AST *ast);

#endif // __LUVASCRIPT_TYPECHECKER_H__