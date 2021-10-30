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
 * @param parent_scope 
 */
void scope_evaluate_statement(Statement *stmt, Scope *parent_scope);

// ----------------------------------------------------------------

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
 * @brief get the base pointer offset on the stack where the var is located
 * 
 * @param scope the current scope
 * @param var_name the name of the variable
 * @return int the offset from the base pointer
 */
int scope_get_variable_rbp_offset(Scope *scope, char *var_name);


// ----------------------------------------------------------------

int scope_contains_variable(Scope *scope, char *var_name);
int scope_contains_local_variable(Scope *scope, char *var_name);
int scope_contains_global_variable(Scope *scope, char *var_name);

char *scope_get_variable_address(Scope *scope, char *var_name);

int scope_contains_function(Scope *scope, char *func_name);

VariableTemplate *scope_get_variable_by_name(Scope *scope, char *var_name);
FunctionTemplate *scope_get_function_by_name(Scope *scope, char *func_name);
