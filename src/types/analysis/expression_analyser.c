#include <types/analysis/analyser_impl.h>

#include <limits.h>

DataType *analyse_expression(DataTypeTable *dtt, Expression_T *expression, Scope *scope) {
    switch (expression->type) {
        case EXPRESSION_TYPE_LITERAL:
            return analyse_literal_expression(dtt, expression->expr.literal_expr, scope);
        case EXPRESSION_TYPE_UNARY:
            return analyse_unary_expression(dtt, expression->expr.unary_expr, scope);
        case EXPRESSION_TYPE_BINARY:
            return analyse_binary_expression(dtt, expression->expr.binary_expr, scope);
        case EXPRESSION_TYPE_NESTED:
            return analyse_expression(dtt, expression->expr.nested_expr->expression, scope);
        case EXPRESSION_TYPE_ASSIGNMENT:
            return analyse_assignment_expression(dtt, expression->expr.assignment_expr, scope);
        case EXPRESSION_TYPE_FUNCTION_CALL:
            return analyse_function_call_expression(dtt, expression->expr.func_call_expr, scope);
        case EXPRESSION_TYPE_MEMBER_ACCESS:
            return analyse_member_access_expression(dtt, expression->expr.member_access_expr, scope);
        case EXPRESSION_TYPE_ARRAY_ACCESS:
            return analyse_array_access_expression(dtt, expression->expr.array_access_expr, scope);
        case EXPRESSION_TYPE_LIST:
            return analyse_list_expression(dtt, expression->expr.list_expr, scope);
    }
    return false;
}

DataType *analyse_literal_expression(DataTypeTable *dtt, Literal_T *literal_expression, Scope *scope) {
    switch (literal_expression->type) {
        case LITERAL_NUMBER: {
            // check if number is a integer or a float
            // TODO: at this point in time, the compiler only supports integers

            // convert string to number
            char *end;
            long long int value = strtoll(literal_expression->value, &end, 10);

            // if INT_MIN <= value <= INT_MAX, then it is an integer
            // as per definition, the compiler defaults to i32, even
            // if the literal is an i16 or i8 (TODO: add link to spec)
            if (value >= INT_MIN && value <= INT_MAX) {
                return data_type_table_get(dtt, "i32");
            } else {
                // the value is to big to fit into an i32, so it is a i64
                return data_type_table_get(dtt, "i64");
            }
        }
        case LITERAL_STRING: {
            return data_type_table_get(dtt, "string");
        }
        case LITERAL_BOOLEAN: {
            return data_type_table_get(dtt, "bool");
        }
        case LITERAL_CHARACTER: {
            return data_type_table_get(dtt, "char");
        }
        case LITERAL_IDENTIFIER: {
            // get the variable from the scope
            VariableTemplate *vt = scope_get_variable_by_name(scope, literal_expression->value);

            // if the variable is not found, then it is an error
            if (vt == NULL) {
                printf("%s:%d:%d: " RED "error: " RESET "undefined variable '%s'\n",
                       "TODO", 0, 0,
                       literal_expression->value);
                return NULL;
            }

            // return the datatype of the variable
            return data_type_table_get(dtt, vt->type_identifier);
        }
    }
    return NULL;
}

DataType *analyse_unary_expression(DataTypeTable *dtt, UnaryExpression_T *unary_expression, Scope *scope) {
    // get the datatype of the operand
    DataType *operand_type = analyse_literal_expression(dtt, unary_expression->identifier, scope);
    
    // in the current version of the compiler, the datatype 
    // of the unary expression is always the datatype of
    // the identifier
    // This could be changed in the future, when new unary
    // operators are added to the language (i.e. casting [x as i32])
    return operand_type;
}

DataType *analyse_binary_expression(DataTypeTable *dtt, BinaryExpression_T *binary_expression, Scope *scope) {
    DataType *left_type = analyse_expression(dtt, binary_expression->expression_left, scope);
    DataType *right_type = analyse_expression(dtt, binary_expression->expression_right, scope);

    // if the left or right expression is NULL, then it is an error
    if (left_type == NULL || right_type == NULL) {
        return NULL;
    }

    // check if the types are compatible
    if (!data_type_is_compatible(left_type, right_type)) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               left_type->identifier, right_type->identifier);
        return NULL;
    }

    // if the types have different sizes (but are compatible), then the result is a larger type
    if (left_type->size < right_type->size) {
        return right_type;
    } else {
        return left_type;
    }
}

DataType *analyse_assignment_expression(DataTypeTable *dtt, AssignmentExpression_T *assignment_expression, Scope *scope) {
    DataType *left_type = analyse_expression(dtt, assignment_expression->identifier, scope);
    DataType *right_type = analyse_expression(dtt, assignment_expression->assignment_value, scope);

    // if the left or right expression is NULL, then it is an error
    if (left_type == NULL || right_type == NULL) {
        return NULL;
    }

    // check if the types are compatible
    if (!data_type_is_compatible(left_type, right_type)) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               left_type->identifier, right_type->identifier);
        return NULL;
    }

    // if the types have different sizes (but are compatible),
    // then the size of the left datatype must be bigger than
    // the right datatype, else throw an error
    if (left_type->size < right_type->size) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               left_type->identifier, right_type->identifier);
        return NULL;
    }

    return left_type;
}

DataType *analyse_function_call_expression(DataTypeTable *dtt, FunctionCallExpression_T *call_expression, Scope *scope) {
    // check if the scope contains a function with the given identifier
    if (!scope_contains_function(scope, call_expression->function_identifier)) {
        printf("%s:%d:%d: " RED "error: " RESET "function '%s' not defined\n", "TODO", 0, 0, call_expression->function_identifier);
        return NULL;
    }

    // allocate an array of datatype pointers
    // this will temporarily hold the types of the argument expressions
    DataType **parameter_types = calloc(arraylist_size(call_expression->argument_expression_list->expressions), sizeof(DataType *));

    // iterate over the argument expressions and analyse them
    for (int i = 0; i < arraylist_size(call_expression->argument_expression_list->expressions); i++) {
        Expression_T *expression = arraylist_get(call_expression->argument_expression_list->expressions, i);
        DataType *parameter_type = analyse_expression(dtt, expression, scope);
        if (parameter_type == NULL) {
            free(parameter_types);
            return NULL;
        }
        parameter_types[i] = parameter_type;
    }

    // loop through all functions that are defined in the scope and check if the parameter types match
    for (size_t i = 0; i < scope->function_templates->size; i++) {
        FunctionTemplate *function = arraylist_get(scope->function_templates, i);
        // check if the identifier matches
        if (strcmp(function->identifier, call_expression->function_identifier) != 0) {
            continue;
        }

        // check if the parameter count matches
        if (arraylist_size(function->parameter_types) != arraylist_size(call_expression->argument_expression_list->expressions)) {
            continue;
        }

        // check if the parameter types match
        for (size_t j = 0; j < arraylist_size(function->parameter_types); j++) {
            char *parameter_type_identifier = arraylist_get(function->parameter_types, j);
            DataType *parameter_type = data_type_table_get(dtt, parameter_type_identifier);
            DataType *argument_type = parameter_types[j];

            // check if the types of the argument expression and the parameter
            // are compatible with each other
            // for more information about type compatibility see:
            // - the documentation: TODO: insert link
            // - the function description of `data_type_is_compatible()` in src/types/datatype.c
            if (!data_type_is_compatible(parameter_type, argument_type)) {
                // the types are not compatible
                // check next function
                goto next_function;
            }

            // check if the size of the argument exppression type is smaller or
            // equal to the size of the function parameter type
            if (parameter_type->size < argument_type->size) {
                // the argument expression type is smaller than the parameter type
                // check next function
                goto next_function;
            }
        }

        // if we reach this point, the function call is valid
        // free the temporary array of parameter types
        free(parameter_types);

        // assign the id of the function to the function call
        // to simplify code generation
        // without it the generator would have to look up which
        // function is the correct one, but since we already did
        // that in the previous loop, we can just assign the id
        call_expression->id = function->id;

        // return the return type of the function
        return data_type_table_get(dtt, function->return_type);

        next_function: {
            // types do not match
            continue;
        }
    }

    // if we reach this point, the function call is invalid
    printf("%s:%d:%d: " RED "error: " RESET "function '%s()' not defined\n", "TODO", 0, 0, call_expression->function_identifier);

    // free the temporary array of parameter types
    free(parameter_types);

    return NULL;
}

DataType *analyse_member_access_expression(DataTypeTable *dtt, MemberAccessExpression_T *member_access_expression, Scope *scope) {
    return NULL; // TODO: implement
}

DataType *analyse_array_access_expression(DataTypeTable *dtt, ArrayAccessExpression_T *array_access_expression, Scope *scope) {
    return NULL; // TODO: implement
}

DataType *analyse_list_expression(DataTypeTable *dtt, ExpressionList_T *list_expression, Scope *scope) {
    DataType *type = NULL;

    // loop through all expressions in the list and analyse them
    for (size_t i = 0; i < arraylist_size(list_expression->expressions); i++) {
        Expression_T *expression = arraylist_get(list_expression->expressions, i);
        DataType *expression_type = analyse_expression(dtt, expression, scope);
        if (expression_type == NULL) {
            return NULL;
        }
        if (type == NULL) {
            type = expression_type;
        } else if (!data_type_is_compatible(type, expression_type)) {
            printf("%s:%d:%d: " RED "error: " RESET "incompatible types in list expression\n", "TODO", 0, 0);
            return NULL;
        }

        // types are compatible
        // check if one needs more bytes than the other
        if (type->size < expression_type->size) {
            type = expression_type;
        }
    }

    return type;
}
