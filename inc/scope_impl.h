#include <scope.h>
#include <types/ast.h>
#include <types/statement.h>

void eval_ast(AST *ast);
void eval_statement(Statement *stmt, Scope *global_scope);