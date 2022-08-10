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

    return table;
}

bool load_user_defined_types(DataTypeTable *dtt, AST *ast) {

    // loop over all packages and ckeck if they contain a (new) user defined type
    for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
        Package *package = arraylist_get(ast->packages, i);

        // enum definitions
        for (size_t j = 0; j < arraylist_size(package->enum_definitions); j++) {
            EnumDefinition *enum_definition = arraylist_get(package->enum_definitions, j);

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
    return false;
}

bool analyse_variable(DataTypeTable *dtt, Variable *variable, Scope *scope) {
    DataType *type_of_expression = analyse_expression(dtt, variable->initializer, scope);

    if (type_of_expression == NULL) {
        return false;
    }

    DataType *type_of_variable = data_type_table_get(dtt, variable->type_identifier);

    if (type_of_variable == NULL) {
        printf("%s:%d:%d: " RED "error: " RESET "type '%s' not defined\n", "TODO", 0, 0, variable->type_identifier);
        return false;
    }

    if (!data_type_is_compatible(type_of_variable, type_of_expression)) {
        printf("%s:%d:%d: " RED "error: " RESET "type '%s' is not compatible with type '%s'\n", "TODO", 0, 0, 
                type_of_expression->identifier, type_of_variable->identifier);
        return false;
    }

    return true;
}
