#ifndef __LUVA_NODE_ENUM_H__
#define __LUVA_NODE_ENUM_H__

#include <util/arraylist.h>

typedef struct _enum_definition EnumDefinition;
typedef struct _enum_definition_member EnumDefinitionMember;

// const int ENUM_MEMBER_SIZE_BYTES = 4; // 4 bytes (32-bit)

struct _enum_definition {
    // the name of the enum
    char *name;
    // ArrayList<EnumDefinitionMember>
    ArrayList *members;
};

struct _enum_definition_member {
    // the name of the enum field
    char *name;
    // the value of the enum field
    int value;
};

EnumDefinition *enum_definition_new();
EnumDefinition *enum_definition_copy(EnumDefinition *enum_definition);
void enum_definition_free(EnumDefinition *enum_definition);

EnumDefinitionMember *enum_definition_member_new(char *name, int value);
EnumDefinitionMember *enum_definition_member_copy(EnumDefinitionMember *enum_definition_member);
void enum_definition_member_free(EnumDefinitionMember *enum_definition_member);

extern const int ENUM_MEMBER_SIZE_BYTES;

#endif // __LUVA_NODE_ENUM_H__