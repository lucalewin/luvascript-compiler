#include <scope.h>
#include <types/ast.h>
#include <types/statement.h>
#include <function.h>

/**
 * @brief 
 * 
 * @param ast 
 */
void scope_evaluate_ast(AST *ast);

/**
 * @brief 
 * 
 * @param function 
 */
void scope_evaluate_function(Function *function);

/**
 * @brief 
 * 
 * @param stmt 
 * @param global_scope 
 */
void scope_evaluate_statement(Statement *stmt);

/**
 * @brief 
 * 
 * @param stmt 
 * @param global_scope 
 */
void scope_evaluate_statement_old(Statement *stmt, Scope *global_scope);

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
 * @brief check if the scope contains the function
 * 
 * @param scope the scope
 * @param name the name of the function
 * @return int 1 (true) if the function is in the scope, otherwise 0 (false)
 */
int scope_contains_function_name(Scope *scope, char *func_name);

/**
 * @brief add a variable to the scope
 * 
 * @param scope the scope
 * @param var the variable
 */
void scope_add_variable(Scope *scope, Variable *var);

/**
 * @brief add a function to the scope
 * 
 * @param scope the current scoep
 * @param function the function to be added to the scope
 */
void scope_add_function(Scope *scope, Function *function);

void scope_remove_variable(Scope *scope, char *var_name);

/**
 * @brief get the base pointer offset on the stack where the var is located
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int the offset from the base pointer
 */
int scope_get_variable_rbp_offset(Scope *scope, char *var_name);

/**
 * @brief get the variable in the scope with the specified name
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return Variable* the pointer of the variable
 */
Variable *scope_get_variable_by_name(Scope *scope, char *var_name);

/**
 * @brief get the function in the current scope with the specified name
 * 
 * @param scope the current scope
 * @param func_name the name of the function
 * @return Function* the pointer of the function if the function exists in the current scope, else NULL
 */
Function *scope_get_function_by_name(Scope *scope, char *func_name);