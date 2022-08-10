#ifndef __LUVASCRIPT_TYPES_ANALYSER_IMPL_H__
#define __LUVASCRIPT_TYPES_ANALYSER_IMPL_H__

#include <stdbool.h>

#include <types/datatype.h>
#include <types/table.h>

#include <parsing/scope.h>
#include <parsing/scope_impl.h>
#include <parsing/nodes/ast.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/expression.h>


bool load_user_defined_types(DataTypeTable *dtt, AST *ast);

bool analyse_ast(DataTypeTable *dtt, AST *ast);
bool analyse_package(DataTypeTable *dtt, Package *package);
bool analyse_function(DataTypeTable *dtt, Function *function);
bool analyse_statement(DataTypeTable *dtt, Statement *statement);
bool analyse_variable(DataTypeTable *dtt, Variable *variable, Scope *scope);

DataType *analyse_expression(DataTypeTable *dtt, Expression_T *expression, Scope *scope);

DataType *analyse_literal_expression(DataTypeTable *dtt, Literal_T *literal_expression, Scope *scope);
DataType *analyse_unary_expression(DataTypeTable *dtt, UnaryExpression_T *unary_expression, Scope *scope);
DataType *analyse_binary_expression(DataTypeTable *dtt, BinaryExpression_T *binary_expression, Scope *scope);
DataType *analyse_assignment_expression(DataTypeTable *dtt, AssignmentExpression_T *assignment_expression, Scope *scope);
DataType *analyse_function_call_expression(DataTypeTable *dtt, FunctionCallExpression_T *call_expression, Scope *scope);
DataType *analyse_member_access_expression(DataTypeTable *dtt, MemberAccessExpression_T *member_access_expression, Scope *scope);
DataType *analyse_array_access_expression(DataTypeTable *dtt, ArrayAccessExpression_T *array_access_expression, Scope *scope);
DataType *analyse_list_expression(DataTypeTable *dtt, ExpressionList_T *list_expression, Scope *scope);

#endif