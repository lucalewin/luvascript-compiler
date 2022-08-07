#ifndef LUVA_DEBUG_H
#define LUVA_DEBUG_H

#include <parsing/nodes/literal.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/variable.h>

void print_literal(Literal_T *literal);
void print_expression(Expression_T *expression);
void print_statement(Statement *statement);
void print_variable(Variable *var);

#endif // LUVA_DEBUG_H