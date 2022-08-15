#ifndef LUVA_x86_64_OPTIMIZER_H
#define LUVA_x86_64_OPTIMIZER_H

#include <parsing/nodes/expression.h>

Expression *simplify_expression(Expression *expr);

#endif // LUVA_x86_64_OPTIMIZER_H