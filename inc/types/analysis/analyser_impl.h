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

bool analyse_ast       (DataTypeTable *dtt, AST *ast);
bool analyse_package   (DataTypeTable *dtt, Package *package);
bool analyse_function  (DataTypeTable *dtt, Function *function);
bool analyse_statement (DataTypeTable *dtt, Statement *statement);
bool analyse_variable  (DataTypeTable *dtt, Variable *variable, Scope *scope);

bool analyse_expression               (DataTypeTable *dtt, Expression *expression,                Scope *scope);
bool analyse_literal                  (DataTypeTable *dtt, Literal *literal,                      Scope *scope);
bool analyse_unary_expression         (DataTypeTable *dtt, UnaryExpression *unary,                Scope *scope);
bool analyse_binary_expression        (DataTypeTable *dtt, BinaryExpression *binary,              Scope *scope);
bool analyse_function_call_expression (DataTypeTable *dtt, FunctionCallExpression *function_call, Scope *scope);
bool analyse_assignment_expression    (DataTypeTable *dtt, AssignmentExpression *assignment,      Scope *scope);
bool analyse_array_access_expression  (DataTypeTable *dtt, ArrayAccessExpression *array_access,   Scope *scope);
bool analyse_member_access_expression (DataTypeTable *dtt, MemberAccessExpression *member_access, Scope *scope);
bool analyse_expression_list          (DataTypeTable *dtt, ExpressionList *list,                  Scope *scope);


// DataType *analyse_expression(DataTypeTable *dtt, Expression *expression, Scope *scope);

// DataType *analyse_literal_expression(DataTypeTable *dtt, Literal *literal_expression, Scope *scope);
// DataType *analyse_unary_expression(DataTypeTable *dtt, UnaryExpression *unary_expression, Scope *scope);
// DataType *analyse_binary_expression(DataTypeTable *dtt, BinaryExpression *binary_expression, Scope *scope);
// DataType *analyse_assignment_expression(DataTypeTable *dtt, AssignmentExpression *assignment_expression, Scope *scope);
// DataType *analyse_function_call_expression(DataTypeTable *dtt, FunctionCallExpression *call_expression, Scope *scope);
// DataType *analyse_member_access_expression(DataTypeTable *dtt, MemberAccessExpression *member_access_expression, Scope *scope);
// DataType *analyse_array_access_expression(DataTypeTable *dtt, ArrayAccessExpression *array_access_expression, Scope *scope);
// DataType *analyse_list_expression(DataTypeTable *dtt, ExpressionList *list_expression, Scope *scope);

#endif