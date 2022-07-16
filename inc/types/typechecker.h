#ifndef __LUVASCRIPT_TYPECHECKER_H__
#define __LUVASCRIPT_TYPECHECKER_H__

#include <stdbool.h>

#include <parsing/nodes/ast.h>

/**
 * @brief TODO(lucalewin) add description
 * 
 * @param ast 
 * @return true 
 */
bool check_types(AST *ast);

#endif // __LUVASCRIPT_TYPECHECKER_H__