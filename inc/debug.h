#ifndef LUVA_DEBUG_H
#define LUVA_DEBUG_H

#include "expression.h"
#include "statement.h"
#include "function.h"

void debugPrintTab(int amount);

void debugPrintExpression(Expr *expr, int depth);
void debugPrintStatement(Statement *st, int depth);
void debugPrintFuncParam(FuncParam *func_param, int depth);
void debugPrintFuncReturnType(FuncReturnType *return_type, int depth);
void debugPrintFunction(Function *func, int depth);

void debugPrintType(DataType *datatype, int depth);

#endif // LUVA_DEBUG_H