#ifndef LUVA_X86_64_H
#define LUVA_X86_64_H

#include "../include/util.h"
#include "../include/arraylist.h"
#include "../include/token.h"
#include "../include/type.h"
#include "../include/expression.h"
#include "../include/statement.h"
#include "../include/function.h"
#include "../include/ast.h"

char *parser_convert_ast_to_assembly(AST *ast);
char *parser_convert_function_to_assembly(Function *func);
char *parser_convert_statement_to_assembly(Statement *stmt);
char *parser_convert_expression_to_assembly(Expr* expr);

#endif