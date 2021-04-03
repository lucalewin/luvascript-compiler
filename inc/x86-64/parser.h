#ifndef LUVA_X86_64_H
#define LUVA_X86_64_H

#include <util.h>
#include <arraylist.h>
#include <type.h>
#include <expression.h>
#include <statement.h>
#include <function.h>
#include <ast.h>
#include <x86-64/context.h>

char *parser_convert_function_to_assembly(Function *func);
char *parser_convert_statement_to_assembly(Statement *stmt);
char *parser_convert_expression_to_assembly(Expr* expr);

#endif