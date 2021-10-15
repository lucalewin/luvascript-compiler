#ifndef LUVA_DEBUG_H
#define LUVA_DEBUG_H

#include <literal.h>
#include <expression.h>
#include <statement.h>

void print_literal(Literal_T *literal);
void print_expression(Expression_T *expression);
void print_statement(Statement *statement);

#endif // LUVA_DEBUG_H