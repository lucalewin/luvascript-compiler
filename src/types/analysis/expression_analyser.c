#include <types/analysis/analyser_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <util/logging/logger.h>

bool analyse_expression(DataTypeTable *dtt, Expression *expression, Scope *scope) {
    switch (expression->type) {
        case EXPRESSION_TYPE_LITERAL:
            return analyse_literal(dtt, expression->expr.literal, scope);
        case EXPRESSION_TYPE_UNARY:
            return analyse_unary_expression(dtt, expression->expr.unary, scope);
        case EXPRESSION_TYPE_BINARY:
            return analyse_binary_expression(dtt, expression->expr.binary, scope);
        case EXPRESSION_TYPE_NESTED:
            return analyse_expression(dtt, expression->expr.nested, scope);
        case EXPRESSION_TYPE_ASSIGNMENT:
            return analyse_assignment_expression(dtt, expression->expr.assignment, scope);
        case EXPRESSION_TYPE_FUNCTION_CALL:
            return analyse_function_call_expression(dtt, expression->expr.function_call, scope);
        case EXPRESSION_TYPE_MEMBER_ACCESS:
            return analyse_member_access_expression(dtt, expression->expr.member_access, scope);
        case EXPRESSION_TYPE_ARRAY_ACCESS:
            return analyse_array_access_expression(dtt, expression->expr.array_access, scope);
        case EXPRESSION_TYPE_LIST:
            return analyse_expression_list(dtt, expression->expr.list, scope);
        default:
            printf("%s:%d:%d: " RED "error: " RESET "unknown expression type %d\n", "TODO", 0, 0, expression->type);
            return false;
    }
    return false;
}

bool analyse_literal(DataTypeTable *dtt, Literal *literal, Scope *scope) {
    // FIXME: replace data_type_init with some sort of datatype copy
    switch (literal->type) {
        case LITERAL_NUMBER: {
            // check if number is a integer or a float
            // TODO: at this point in time, the compiler only supports integers

            // convert string to number
            char *end;
            long long int value = strtoll(literal->value, &end, 10);

            // if INT_MIN <= value <= INT_MAX, then it is an integer
            // as per definition, the compiler defaults to i32, even
            // if the literal is an i16 or i8 (TODO: add link to spec)
            if (value >= INT_MIN && value <= INT_MAX) {
                literal->datatype = data_type_init("i32", 4, DATA_TYPE_INT, false, false, 0, 0);
            } else {
                // the value is to big to fit into an i32, so it is a i64
                literal->datatype = data_type_init("i64", 8, DATA_TYPE_INT, false, false, 0, 0);
            }
            return true;
        }
        case LITERAL_STRING: {
            size_t string_length = strlen(literal->value);
            literal->datatype = data_type_init("string", 1, DATA_TYPE_CHAR, true, false, string_length, 0);
            return true;
        }
        case LITERAL_CHARACTER:
            literal->datatype = data_type_init("char", 1, DATA_TYPE_CHAR, false, false, 0, 0);
            return true;
        case LITERAL_IDENTIFIER: {
            // check if the identifier is an enum or variable
            if (data_type_table_contains(dtt, literal->value)) {
                DataType *datatype = data_type_table_get(dtt, literal->value);

                // check if the identifier is an enum
                if (datatype->type != DATA_TYPE_ENUM) {
                    printf("%s:%d:%d: " RED "error: " RESET "type identifier '%s' is not an enum\n", "TODO", 0, 0, literal->value);
                    return false;
                }

                literal->datatype = data_type_copy(datatype); 
                return true;
            }
            // else: the identifier is a variable

            Variable *variable = scope_get_variable_by_name(scope, literal->value);
            if (variable == NULL) {
                printf("%s:%d:%d: " RED "error: " RESET "undefined variable '%s'\n",
                       "TODO", 0, 0, literal->value);
                return false;
            }

            // copy the datatype of the variable to the literal
            literal->datatype = data_type_init(variable->type->identifier, variable->type->size,
                    variable->type->type, variable->type->is_array,
                    variable->type->is_pointer, variable->type->array_size,
                    variable->type->pointer_depth);
            return true;
        }
        case LITERAL_BOOLEAN:
            literal->datatype = data_type_init("bool", 1, DATA_TYPE_BOOL, false, false, 0, 0);
            return true;
    }
    return false;
}

bool analyse_unary_expression(DataTypeTable *dtt, UnaryExpression *unary, Scope *scope) {
    // analyse the operand
    if (!analyse_literal(dtt, unary->identifier, scope)) {
        return false;
    }
    
    // in the current version of the compiler, the datatype 
    // of the unary expression is always the datatype of
    // the identifier
    // This could be changed in the future, when new unary
    // operators are added to the language (i.e. casting [x as i32])
    unary->datatype = data_type_copy(unary->identifier->datatype);

    return true;
}

bool analyse_binary_expression(DataTypeTable *dtt, BinaryExpression *binary, Scope *scope) {
    // analyse left expression
    if (!analyse_expression(dtt, binary->left, scope)) {
        return false;
    }

    // analyse right expression
    if (!analyse_expression(dtt, binary->right, scope)) {
        return false;
    }

    DataType *left_datatype = expression_get_datatype(binary->left);
    DataType *right_datatype = expression_get_datatype(binary->right);

    // check if the types are compatible
    if (!data_type_is_compatible(left_datatype, right_datatype)) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               left_datatype->identifier, right_datatype->identifier);
        return false;
    }

    // if the types have different sizes (but are compatible), then the result is a larger type 
    binary->datatype = left_datatype->size < right_datatype->size ? 
            data_type_copy(right_datatype) : data_type_copy(left_datatype);

    return true;
}

bool analyse_function_call_expression(DataTypeTable *dtt, FunctionCallExpression *function_call, Scope *scope) {
    // check if the scope contains a function with the given identifier
    if (!scope_contains_function(scope, function_call->function_identifier)) {
        printf("%s:%d:%d: " RED "error: " RESET "function '%s' not defined\n",
                "TODO", 0, 0, function_call->function_identifier);
        return NULL;
    }

    // allocate an array of datatype pointers
    // this will temporarily hold the types of the argument expressions
    DataType **parameter_types = calloc(arraylist_size(function_call->argument_expression_list->expressions), sizeof(DataType *));

    // iterate over the argument expressions and analyse them
    for (int i = 0; i < arraylist_size(function_call->argument_expression_list->expressions); i++) {
        Expression *expression = arraylist_get(function_call->argument_expression_list->expressions, i);

        if (!analyse_expression(dtt, expression, scope)) {
            free(parameter_types);
            return false;
        }

        DataType *parameter_type = expression_get_datatype(expression);

        if (parameter_type == NULL) {
            free(parameter_types);
            return false;
        }

        parameter_types[i] = parameter_type;
    }

    // loop through all functions that are defined in the scope and check if the parameter types match
    for (size_t i = 0; i < scope->functions->size; i++) {
        Function *function = arraylist_get(scope->functions, i);
        // check if the identifier matches

        log_debug("checking function '%s'\n", function->identifier);

        if (strcmp(function->identifier, function_call->function_identifier) != 0) {
            continue;
        }

        // check if the parameter count matches
        if (arraylist_size(function->parameters) != arraylist_size(function_call->argument_expression_list->expressions)) {
            continue;
        }

        // check if the parameter types match
        for (size_t j = 0; j < arraylist_size(function->parameters); j++) {
            Variable *parameter = arraylist_get(function->parameters, j);
            // char *parameter_type_identifier = arraylist_get(function->parameters, j);
            // DataType *parameter_type = data_type_table_get(dtt, parameter_type_identifier);
            DataType *argument_type = parameter_types[j];

            // check if the types of the argument expression and the parameter
            // are compatible with each other
            // for more information about type compatibility see:
            // - the documentation: TODO: insert link
            // - the function description of `data_type_is_compatible()` in src/types/datatype.c
            if (!data_type_is_compatible(parameter->type, argument_type)) {
                // the types are not compatible
                // check next function
                goto next_function;
            }

            // check if the size of the argument exppression type is smaller or
            // equal to the size of the function parameter type
            if (parameter->type->size < argument_type->size) {
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
        function_call->id = function->id;

        // set the type of the expression to the return
        // type of the function
        function_call->datatype = data_type_copy(function->return_type);
        
        return true;

        next_function: {
            // types do not match
            continue;
        }
    }

    // if we reach this point, the function call is invalid
    printf("%s:%d:%d: " RED "error: " RESET "function '%s()' not defined\n", "TODO", 0, 0, function_call->function_identifier);

    // free the temporary array of parameter types
    free(parameter_types);

    return false;
}

bool analyse_assignment_expression(DataTypeTable *dtt, AssignmentExpression *assignment, Scope *scope) {
    if (!analyse_expression(dtt, assignment->identifier, scope)) {
        return false;
    }

    if (!analyse_expression(dtt, assignment->assignment_value, scope)) {
        return false;
    }

    DataType *identifier_datatype = expression_get_datatype(assignment->identifier);
    DataType *assignment_value_datatype = expression_get_datatype(assignment->assignment_value);

    // check if the types are compatible
    if (!data_type_is_compatible(identifier_datatype, assignment_value_datatype)) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               identifier_datatype->identifier, assignment_value_datatype->identifier);
        return false;
    }

    // if the types have different sizes (but are compatible),
    // then the size of the left datatype must be bigger than
    // the right datatype, else throw an error
    if (identifier_datatype->size < assignment_value_datatype->size) {
        printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
               "TODO", 0, 0,
               identifier_datatype->identifier, assignment_value_datatype->identifier);
        return false;
    }

    assignment->datatype = data_type_copy(identifier_datatype);
    return true;
}

bool analyse_array_access_expression(DataTypeTable *dtt, ArrayAccessExpression *array_access, Scope *scope) {
    if (!analyse_literal(dtt, array_access->identifier, scope)) {
        return false;
    }
    
    if (!analyse_expression(dtt, array_access->index_expression, scope)) {
        return false;
    }

    DataType *identifier_datatype = array_access->identifier->datatype;
    DataType *index_expression_datatype = expression_get_datatype(array_access->index_expression);

    // check if the index expression is an (unsigned) integer
    if (index_expression_datatype->type != DATA_TYPE_INT && index_expression_datatype->type != DATA_TYPE_UNSIGNED_INT) {
        printf("%s:%d:%d: " RED "error: " RESET "index expression must be an integer\n",
               "TODO", 0, 0);
        return false;
    }

    // check if the identifier is an array
    if (!identifier_datatype->is_array) {
        printf("%s:%d:%d: " RED "error: " RESET "identifier must be an array\n",
               "TODO", 0, 0);
        return false;
    }

    // SPECIAL CASE: string
    if (strcmp(identifier_datatype->identifier, "string") == 0) {
        array_access->datatype = data_type_init("char", 1, DATA_TYPE_CHAR, false, false, 0, 0);
    } else {
        array_access->datatype = data_type_copy(identifier_datatype);
        array_access->datatype->is_array = false;
        array_access->datatype->array_size = 0;
    }

    return true;
}

bool analyse_member_access_expression(DataTypeTable *dtt, MemberAccessExpression *member_access, Scope *scope) {
    if (!analyse_expression(dtt, member_access->identifier, scope)) {
        return false;
    }

    DataType *identifier_datatype = expression_get_datatype(member_access->identifier);

    if (identifier_datatype->type != DATA_TYPE_ENUM) {
        printf("%s:%d:%d: " RED "error: " RESET "member access expressions currently only support enums\n",
               "TODO", 0, 0);
        return false;
    }

    member_access->datatype = data_type_copy(identifier_datatype);

    log_debug("mae dt: %s\n", identifier_datatype->identifier);

    return true;
}

bool analyse_expression_list(DataTypeTable *dtt, ExpressionList *list, Scope *scope) {
    // analyse all expressions in the list
    for (size_t i = 0; i < arraylist_size(list->expressions); i++) {
        if (!analyse_expression(dtt, arraylist_get(list->expressions, i), scope)) {
            return false;
        }
    }

    DataType *datatype = NULL;

    // check if all expressions have the same type
    for (size_t i = 0; i < arraylist_size(list->expressions); i++) {
        Expression *expression = arraylist_get(list->expressions, i);
        DataType *expression_datatype = expression_get_datatype(expression);
        if (datatype == NULL) {
            datatype = expression_datatype;
        } else if (!data_type_is_compatible(datatype, expression_datatype)) {
            printf("%s:%d:%d: " RED "error: " RESET "incompatible types '%s' and '%s'\n",
                   "TODO", 0, 0,
                   datatype->identifier, expression_datatype->identifier);
            return false;
        }

        // types are compatible
        // check if one needs more bytes than the other
        if (datatype->size < expression_datatype->size) {
            datatype = expression_datatype;
        }
    }

    list->datatype = data_type_copy(datatype);
    return true;
}
