#include <types/import.h>

#include <string.h>
#include <stdbool.h>

#include <util/util.h>
#include <util/arraylist.h>

#include <logging/logger.h>

ImportDeclaration *import_declaration_new() {
    ImportDeclaration *import_declaration = (ImportDeclaration *) malloc(sizeof(ImportDeclaration));
    import_declaration->package_names = arraylist_create();
    import_declaration->type_identifiers = arraylist_create();
    return import_declaration;
}

ImportDeclaration *import_declaration_copy(ImportDeclaration *import_declaration) {
    ImportDeclaration *new_import_declaration = import_declaration_new();

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

    for (size_t i = 0; i < arraylist_size(import_declarations); i++) {
        ImportDeclaration *import_declaration = (ImportDeclaration *) arraylist_get(import_declarations, i);

        // check if compacted_import_declarations already contains an import with the same package name
        bool already_contains_import = false;
        ImportDeclaration *current = NULL;
        for (size_t j = 0; j < arraylist_size(compacted_import_declarations); j++) {
            ImportDeclaration *compacted_import_declaration = (ImportDeclaration *) arraylist_get(compacted_import_declarations, j);

            // TODO: check whole package name
            if (strcmp(arraylist_get(import_declaration->package_names, 0), arraylist_get(compacted_import_declaration->package_names, 0)) == 0) {
                already_contains_import = true;
                current = compacted_import_declaration;
                break;
            }
        }

        if (already_contains_import) {
            // add type identifiers to existing import
            for (size_t j = 0; j < arraylist_size(import_declaration->type_identifiers); j++) {
                char *type_identifier = arraylist_get(import_declaration->type_identifiers, j);

                /// `*` is a special case, it means all types in the
                /// package (e.g. `ìmport std.io::*;`) should be imported
                /// 
                /// the compiler knows that is should import all types if
                /// `current->type_identifiers` is empty
                if (strcmp(type_identifier, "*") == 0) {
                    // free all type identifiers from `current`
                    for (size_t k = 0; k < arraylist_size(current->type_identifiers); k++) {
                        char *type_identifier = arraylist_get(current->type_identifiers, k);
                        free(type_identifier);
                    }

                    // clear arraylist (size = 0)
                    arraylist_clear(current->type_identifiers);
                } else {
                    arraylist_add(current->type_identifiers, strdup(type_identifier));
                }
            }
        } else {
            // add the import declaration to `compacted_import_declarations`
            arraylist_add(compacted_import_declarations, import_declaration_copy(import_declaration));
        }
    }

    log_warning("compacted %zu to %zu imports\n", arraylist_size(import_declarations), arraylist_size(compacted_import_declarations));

    return compacted_import_declarations;
}

void import_declaration_free(ImportDeclaration *import_declaration) {
    if (import_declaration == NULL) {
        return;
    }
    arraylist_free(import_declaration->package_names);
    arraylist_free(import_declaration->type_identifiers);
    free(import_declaration);
}
