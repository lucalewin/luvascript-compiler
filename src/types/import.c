#include <types/import.h>

#include <string.h>
#include <stdbool.h>

#include <util/util.h>
#include <util/arraylist.h>

#include <logging/logger.h>

ImportDeclaration *import_declaration_new() {
    ImportDeclaration *import_declaration = (ImportDeclaration *) malloc(sizeof(ImportDeclaration));
    import_declaration->package_names = arraylist_create();
    import_declaration->package_name = NULL;
    import_declaration->type_identifiers = arraylist_create();
    return import_declaration;
}

ImportDeclaration *import_declaration_copy(ImportDeclaration *import_declaration) {
    ImportDeclaration *new_import_declaration = import_declaration_new();

    new_import_declaration->package_name = strdup(import_declaration->package_name);

    for (size_t i = 0; i < arraylist_size(import_declaration->package_names); i++) {
        arraylist_add(new_import_declaration->package_names, arraylist_get(import_declaration->package_names, i));
    }

    for (size_t i = 0; i < arraylist_size(import_declaration->type_identifiers); i++) {
        arraylist_add(new_import_declaration->type_identifiers, arraylist_get(import_declaration->type_identifiers, i));
    }

    return new_import_declaration;
}

ArrayList *compact_import_declarations(ArrayList *import_declarations) {
    ArrayList *compacted_import_declarations = arraylist_create();

    bool successful_evaluation = true;

    for (size_t i = 0; i < arraylist_size(import_declarations); i++) {
        ImportDeclaration *import_declaration = (ImportDeclaration *) arraylist_get(import_declarations, i);

        // check if `compact_import_declarations` already
        // contains an import_declaration with the same package_names

        bool compacted_already_contains_package = false;

        ImportDeclaration *compacted_import_declaration = NULL;
        for (size_t j = 0; j < arraylist_size(compacted_import_declarations); j++) {
            compacted_import_declaration = (ImportDeclaration *) arraylist_get(compacted_import_declarations, j);

            if (strcmp(import_declaration->package_name, compacted_import_declaration->package_name) == 0) {
                compacted_already_contains_package = true;
                break;
            }
        }

        if (compacted_already_contains_package) {
            // we found an import_declaration with the same package_names

            if (import_declaration_contains(compacted_import_declaration, "*")) {
                printf("%s:%d:%d: " RED "error: " RESET "cannot import types after using wildcard import '*' for the same package '%s'\n",
                        "TODO", 0, 0, import_declaration->package_name);
                successful_evaluation = false;
                
                goto _exit;
            }

            if (import_declaration_contains(import_declaration, "*")) {
                if (arraylist_size(compacted_import_declaration->type_identifiers) > 0) {
                    printf("%s:%d:%d: " RED "error: " RESET "cannot use wildcard import '*' after importing types from the same package '%s'\n",
                            "TODO", 0, 0, import_declaration->package_name);
                    successful_evaluation = false;
                    
                    goto _exit;
                }
            }

            // check if types are already imported
            for (size_t j = 0; j < arraylist_size(import_declaration->type_identifiers); j++) {
                char *type_ident = (char *) arraylist_get(import_declaration->type_identifiers, j);

                if (import_declaration_contains(compacted_import_declaration, type_ident)) {
                    printf("%s:%d:%d: " RED "error: " RESET "cannot import type '%s' twice from for the same package '%s'\n",
                            "TODO", 0, 0, type_ident, import_declaration->package_name);
                    successful_evaluation = false;

                    goto _exit;
                }

                arraylist_add(compacted_import_declaration->type_identifiers, strdup(type_ident));
            }

            continue;
        } // else: we did not find an import_declaration with the same package_names

        // we didn't find a package_name match, so we can add the import_declaration
        if (import_declaration_contains(import_declaration, "*")) {
            // we found an import_declaration with the wildcard `*`

            // check if import_declaration contains additional type_identifiers
            if (arraylist_size(import_declaration->type_identifiers) > 1) {
                printf("%s:%d:%d: " YELLOW "warning: " RESET "import_declaration contains additional type_identifiers after wildcard '*'\n",
                       "TODO", 0, 0);
            }
        } // else: we found an import_declaration without the wildcard `*`

        // add it to the compacted_import_declarations
        arraylist_add(compacted_import_declarations, import_declaration_copy(import_declaration));
    }

    _exit: ;

    // print all imported types
    // for (size_t i = 0; i < arraylist_size(compacted_import_declarations); i++) {
    //     ImportDeclaration *import_declaration = (ImportDeclaration *) arraylist_get(compacted_import_declarations, i);

    //     printf("%s:%d:%d: " GREEN "info: " RESET "imported types from package '%s':\n",
    //             "TODO", 0, 0, import_declaration->package_name);

    //     for (size_t j = 0; j < arraylist_size(import_declaration->type_identifiers); j++) {
    //         char *type_ident = (char *) arraylist_get(import_declaration->type_identifiers, j);
    //         printf("%s:%d:%d: " GREEN "info: " RESET "  - %s\n",
    //                 "TODO", 0, 0, type_ident);
    //     }
    // }

    // TODO: if successful_evaluation == false, we should free all allocated memory
    
    return successful_evaluation ? compacted_import_declarations : NULL;
}

bool import_declaration_should_import_all(ImportDeclaration *import_declaration) {
    if (arraylist_size(import_declaration->type_identifiers) == 0) {
        return true;
    }

    for (size_t i = 0; i < arraylist_size(import_declaration->type_identifiers); i++) {
        char *type_identifier = arraylist_get(import_declaration->type_identifiers, i);
        if (strcmp(type_identifier, "*") == 0) {
            return true;
        }
    }

    return false;
}

bool import_declaration_contains(ImportDeclaration *import_declaration, char *type_ident) {
    if (import_declaration == NULL || import_declaration->type_identifiers == NULL) {
        return false;
    }
    
    for (size_t i = 0; i < arraylist_size(import_declaration->type_identifiers); i++) {
        char *type_identifier = arraylist_get(import_declaration->type_identifiers, i);
        if (strcmp(type_identifier, type_ident) == 0) {
            return true;
        }
    }

    return false;
}

void import_declaration_free(ImportDeclaration *import_declaration) {
    if (import_declaration == NULL) {
        return;
    }
    arraylist_free(import_declaration->package_names);
    arraylist_free(import_declaration->type_identifiers);
    free(import_declaration);
}
