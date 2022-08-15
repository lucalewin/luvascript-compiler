#include <types/analysis/analyser.h>
#include <types/analysis/analyser_impl.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/logging/logger.h>

#include <parsing/nodes/enum.h>

DataTypeTable *analyse_syntax(AST *ast) {
    if (ast == NULL) {
        return false;
    }

    // create a data type table 
    DataTypeTable *table = data_type_table_new();
    if (table == NULL) {
        return NULL;
    }

    // load user defined types
    if (!load_user_defined_types(table, ast)) {
        data_type_table_free(table);
        return NULL;
    }

    // analyse the syntax tree
    if (!analyse_ast(table, ast)) {
        data_type_table_free(table);
        return NULL;
    }

    return table;
}

bool load_user_defined_types(DataTypeTable *dtt, AST *ast) {

    // loop over all packages and ckeck if they contain a (new) user defined type
    for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
        Package *package = arraylist_get(ast->packages, i);

        // enum definitions
        for (size_t j = 0; j < arraylist_size(package->enum_definitions); j++) {
            Enum *enum_definition = arraylist_get(package->enum_definitions, j);

            if (data_type_table_contains(dtt, enum_definition->name)) {
                printf("%s:%d:%d: " RED "error: " RESET "type '%s' already defined\n", "TODO", 0, 0, enum_definition->name);
                return false;
            }

            if (!data_type_table_add(dtt, enum_definition->name, ENUM_MEMBER_SIZE_BYTES, DATA_TYPE_ENUM)) {
                printf("%s:%d:%d: " RED "error: " RESET "could not add type '%s' to data type table\n", "TODO", 0, 0, enum_definition->name);
                return false;
            }
        }

        // TODO: struct definitions

    }

    return true;
}

bool analyse_ast(DataTypeTable *dtt, AST *ast) {
    if (ast == NULL) {
        return false;
    }
    
    // loop over all packages and ckeck if they contain a (new) user defined type
    for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
        Package *package = arraylist_get(ast->packages, i);

        if (!analyse_package(dtt, package)) {
            return false;
        }
    }

    return true;
}

bool analyse_package(DataTypeTable *dtt, Package *package) {
    if (package == NULL) {
        return false;
    }


    // analyse functions
    for (size_t i = 0; i < arraylist_size(package->functions); i++) {
        Function *function = arraylist_get(package->functions, i);

        if (!analyse_function(dtt, function)) {
            return false;
        }
    }

    // analyse global variables
    for (size_t i = 0; i < arraylist_size(package->global_variables); i++) {
        Variable *variable = arraylist_get(package->global_variables, i);

        if (!analyse_variable(dtt, variable, package->package_scope)) {
            return false;
        }
    }

    return true;
}

bool analyse_function(DataTypeTable *dtt, Function *function) {
    if (function == NULL) {
        return false;
    }

    // TODO: analyse function parameters


    // analyse function body
    for (size_t i = 0; i < arraylist_size(function->statements); i++) {
        Statement *statement = arraylist_get(function->statements, i);

        if (!analyse_statement(dtt, statement)) {
            return false;
        }
    }

    return true;
}

bool analyse_variable(DataTypeTable *dtt, Variable *variable, Scope *scope) {
    // check if the type of the variables datatype is known
    if (variable->type->type == DATA_TYPE_UNKNOWN) {
        // check if the type is defined in the datatype table
        if (data_type_table_contains(dtt, variable->type->identifier)) {
            // set the type of the variable to the type of the datatype
            variable->type = data_type_copy(data_type_table_get(dtt, variable->type->identifier));
        } else {
            // the type is not defined in the datatype table
            printf("%s:%d:%d: " RED "error: " RESET "type '%s' is not defined\n", "TODO", 0, 0, variable->type->identifier);
            return false;
        }
    }
    
    if (!analyse_expression(dtt, variable->initializer, scope)) {
        return false;
    }

    DataType *type_of_expression = expression_get_datatype(variable->initializer);

    if (!data_type_is_compatible(variable->type, type_of_expression)) {
        printf("%s:%d:%d: " RED "error: " RESET "type '%s' is not compatible with type '%s'\n", "TODO", 0, 0, 
                type_of_expression->identifier, variable->type->identifier);
        return false;
    }

    return true;
}
