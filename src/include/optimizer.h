#ifndef LUVA_OPTIMIZER_H
#define LUVA_OPTIMIZER_H

#include "expression.h"
#include "statement.h"

void pre_evaluate_expression(Expr *expr);
void extract_common_expressions(Statement *statement);
void remove_unused_fields(void);

#endif // LUVA_OPTIMIZER_H