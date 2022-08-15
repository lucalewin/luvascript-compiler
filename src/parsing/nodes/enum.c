#include <parsing/nodes/enum.h>

#include <string.h>

#include <util/util.h>

const int ENUM_MEMBER_SIZE_BYTES = 4; // 4 bytes (32-bit)

Enum *enum_definition_new() {
    Enum *enum_definition = malloc(sizeof(Enum));
    enum_definition->name = NULL;
    enum_definition->members = arraylist_create();
    return enum_definition;
}

Enum *enum_definition_copy(Enum *enum_definition) {
    Enum *copy = enum_definition_new();
    copy->name = strdup(enum_definition->name);
    for (size_t i = 0; i < enum_definition->members->size; i++) {
        EnumDefinitionMember *member = arraylist_get(enum_definition->members, i);
        arraylist_add(copy->members, enum_definition_member_copy(member));
    }
    return copy;
}

void enum_definition_free(Enum *enum_definition) {
    if (enum_definition == NULL) {
        return;
    }
    free(enum_definition->name);
    for (size_t i = 0; i < enum_definition->members->size; i++) {
        EnumDefinitionMember *enum_definition_member = arraylist_get(enum_definition->members, i);
        enum_definition_member_free(enum_definition_member);
    }
    arraylist_free(enum_definition->members);
    free(enum_definition);
}

EnumDefinitionMember *enum_definition_member_new(char *name, int value) {
    EnumDefinitionMember *enum_definition_member = malloc(sizeof(EnumDefinitionMember));
    enum_definition_member->name = name;
    enum_definition_member->value = value;
    return enum_definition_member;
}


EnumDefinitionMember *enum_definition_member_get(Enum *enum_definition, char *name) {
    for (size_t i = 0; i < enum_definition->members->size; i++) {
        EnumDefinitionMember *member = arraylist_get(enum_definition->members, i);
        if (strcmp(member->name, name) == 0) {
            return member;
        }
    }
    return NULL;
}

EnumDefinitionMember *enum_definition_member_copy(EnumDefinitionMember *enum_definition_member) {
    return enum_definition_member_new(strdup(enum_definition_member->name), enum_definition_member->value);
}

void enum_definition_member_free(EnumDefinitionMember *enum_definition_member) {
    if (enum_definition_member == NULL) {
        return;
    }
    free(enum_definition_member->name);
    free(enum_definition_member);
}