#include <typechecker.h>

#include <types/ast.h>
#include <types/package.h>
#include <types/function.h>

#include <logging/logger.h>

bool check_for_duplicate_functions(AST *ast);

bool check_types(AST *ast) {
    log_info("Starting type-checking...\n");

    if (check_for_duplicate_functions(ast)) {
        return false;
    }

    log_info("Finnished type-checking\n");
    return true;
}

bool check_for_duplicate_functions(AST *ast) {
    log_debug("Checking for duplicate functions\n");

    bool duplicated_functions_found = false;

    for (int i = 0; i < arraylist_size(ast->packages); i++) {
        Package *package = arraylist_get(ast->packages, i);

        for (int j = 0; j < arraylist_size(package->functions); j++) {
            Function *function = arraylist_get(package->functions, j);

            for (int k = j + 1; k < arraylist_size(package->functions); k++) {
                Function *other_function = arraylist_get(package->functions, k);

                // no need to compare the same function
                if (function == other_function) {
                    continue;
                }

                // check if functions have the same name
                if (strcmp(function->identifier, other_function->identifier) == 0) {
                    // check parameter types

                    // first check if parameter counts match
                    if (arraylist_size(function->parameters) == arraylist_size(other_function->parameters)) {
                        // both functions have the same number of parameters
                        // check if parameter types match
                        for (int l = 0; l < arraylist_size(function->parameters); l++) {
                            Variable *parameter = arraylist_get(function->parameters, l);
                            Variable *other_parameter = arraylist_get(other_function->parameters, l);

                            // check if type identifier of the parameters match
                            if (strcmp(parameter->datatype->type_identifier, other_parameter->datatype->type_identifier) != 0) {
                                // parameters have different types
                                // therefore functions are not the same
                                goto next_other_function;
                            }
                        }

                        printf("    " IRED "error: " RESET "duplicate function '%s' (%s:%d:%d) already defined here (%s:%d:%d)\n", 
                                function->identifier,
                                "TODO", 0, 0,
                                "TODO", 0, 0);

                        duplicated_functions_found = true;
                        goto next_function;
                    }
                    // else: parameter count does not match, so we can't compare parameter types
                    // and therefore these functions are not equal
                }
                next_other_function: ;
            }
            next_function: ;
        }
    }

    return duplicated_functions_found;
}