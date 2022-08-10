#ifndef __LUVASCRIPT_TYPES_ANALYSER_H__
#define __LUVASCRIPT_TYPES_ANALYSER_H__

#include <stdbool.h>

#include <parsing/nodes/ast.h>

#include <types/table.h>

/**
 * @brief TODO: add description
 * 
 * @param ast 
 * @return bool
 */
DataTypeTable *analyse_syntax(AST *ast);

#endif