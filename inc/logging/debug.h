#ifndef LUVA_DEBUG_H
#define LUVA_DEBUG_H

#include <types/literal.h>
#include <types/expression.h>
#include <types/statement.h>

void print_literal(Literal_T *literal);
void print_expression(Expression_T *expression);
void print_statement(Statement *statement);

#endif // LUVA_DEBUG_H