#ifndef __LUVA_TYPE_STRINGS_H__
#define __LUVA_TYPE_STRINGS_H__

typedef struct _TypeString {
    char* id;
    char* value;
} TypeString;

TypeString* create_type_string(char* id, char* value);

char *to_lcc_type_string_identifier(const TypeString *type_string);

void type_string_free(TypeString* type_string);

#endif // __LUVA_TYPE_STRINGS_H__