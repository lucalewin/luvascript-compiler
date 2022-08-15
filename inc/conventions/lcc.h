#ifndef __LUVA_COMPILER_CONVENTION_LCC_H__
#define __LUVA_COMPILER_CONVENTION_LCC_H__

#include <parsing/nodes/variable.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/enum.h>

/**
 * @brief implementation according to the `Luvascript Naming Convention` (LNC)
 * [https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/luvascript-naming-convention/]
 * 
 * @param var the Variable to convert to a lcc identifier
 * @return char* the lcc identifier
 */
char *variable_as_lcc_identifier(Variable *var);

/**
 * @brief implementation according to the `Luvascript Naming Convention` (LNC)
 * [https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/luvascript-naming-convention/]
 * 
 * @param func the Function to convert to a lcc identifier
 * @return char* the lcc identifier
 */
char *function_as_lcc_identifier(Function *func);

/**
 * @brief implementation according to the `Luvascript Naming Convention` (LNC)
 * [https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/luvascript-naming-convention/]
 * 
 * @param func the FunctionTemplate to convert to a lcc identifier
 * @return char* the lcc identifier
 */
// char *functiontemplate_as_lcc_identifier(FunctionTemplate *func);

/**
 * @brief TODO: add description
 * 
 * @param enum_definition 
 * @return char* 
 */
char *enum_definition_as_lcc_identifier(Enum *enum_definition);

/**
 * @brief TODO: add description
 * 
 * @param enum_definition_member 
 * @return char* 
 */
char *enum_definition_member_as_lcc_identifier(EnumDefinitionMember *enum_definition_member);

#endif // __LUVA_COMPILER_CONVENTION_LCC_H__