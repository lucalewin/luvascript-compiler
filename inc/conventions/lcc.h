#ifndef __LUVA_COMPILER_CONVENTION_LCC_H__
#define __LUVA_COMPILER_CONVENTION_LCC_H__

#include <types/variable.h>
#include <types/function.h>

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
 * @param var the VariableTemplate to convert to a lcc identifier
 * @return char* the lcc identifier
 */
char *variabletemplate_as_lcc_identifier(VariableTemplate *var);

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
char *functiontemplate_as_lcc_identifier(FunctionTemplate *func);

#endif // __LUVA_COMPILER_CONVENTION_LCC_H__