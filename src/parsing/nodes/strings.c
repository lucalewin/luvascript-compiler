#include <parsing/nodes/strings.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TypeString* create_type_string(char* id, char* value) {
    TypeString* ts = calloc(1, sizeof(TypeString));
    ts->id = id;
    ts->value = value;
    return ts;
}

char *to_lcc_type_string_identifier(const TypeString *type_string) {
    char *identifier = calloc(strlen(type_string->id) + strlen(type_string->value) + 2 + 5, sizeof(char));
    sprintf(identifier, "_str_%s_%s", type_string->id, type_string->value);
    return identifier;
}

void type_string_free(TypeString* type_string) {
    if (type_string == NULL) {
        return;
    }

    free(type_string->id);
    free(type_string->value);
    free(type_string);
}
