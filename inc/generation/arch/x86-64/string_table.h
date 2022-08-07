#ifndef __LUVA_X86_64_STRING_TABLE_H__
#define __LUVA_X86_64_STRING_TABLE_H__

typedef struct _StringTable StringTable;

StringTable *string_table_new();
void string_table_free(StringTable *table);

void string_table_add(StringTable *table, char *str);
void string_table_add_label(StringTable *table, char *str, char *label);
int string_table_contains(StringTable *table, const char *str);
int string_table_getIndex(StringTable *table, const char *str);
char *string_table_toLNCIdentifier(StringTable *table, char *str);

char *string_table_toString(StringTable *table);

#endif // __LUVA_X86_64_STRING_TABLE_H__