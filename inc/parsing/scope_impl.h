#include <parsing/scope.h>

#include <options.h>
#include <util/arraylist.h>

#include <parsing/nodes/ast.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/enum.h>

/**
 * @brief 
 * 
 * @param options 
 * @param ast 
 * @return int 
 */
int scope_evaluate_ast(CommandlineOptions *options, AST *ast);

/**
 * @brief 
 * 
 * @param package 
 * @return int: 1 if successful, 0 otherwise
 */
int scope_evaluate_package(Package *package);

/**
 * @brief 
 * 
 * @param function 
 */
int scope_evaluate_function(Function *function);

/**
 * @brief 
 * 
 * @param stmt 
 * @param parent_scope 
 */
// void scope_evaluate_statement(Statement *stmt, Scope *parent_scope);
int scope_evaluate_statement(Statement *stmt);

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

Scope *scope_join(Scope *scope, Scope *other);

void scope_free(Scope *scope);

void scope_merge(Scope *dest, Scope *src);

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
int scope_contains_function(Scope *scope, char *func_name);
int scope_contains_enum(Scope *scope, char *enum_name);

VariableTemplate *scope_get_variable_by_name(Scope *scope, char *var_name);
FunctionTemplate *scope_get_function_by_name(Scope *scope, char *func_name);
EnumDefinition *scope_get_enum_by_name(Scope *scope, char *enum_name);

FunctionTemplate *scope_getFunctionByID(Scope *scope, unsigned long long int id);
EnumDefinition *scope_get_enum_by_id(Scope *scope, unsigned long long int id);
