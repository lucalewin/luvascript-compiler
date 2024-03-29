#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_PROGRAM_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_PROGRAM_H__

#include <util/arraylist.h>
#include <generation/arch/x86-64/string_table.h>
#include <generation/arch/x86-64/assembly/section.h>

typedef struct _AssemblyProgram {
    ArrayList *global_directives; // array of strings
    ArrayList *extern_directives; // array of strings

    StringTable *string_table;

    DataSection* data;
    BssSection* bss;
    TextSection* text;
} AssemblyProgram;

AssemblyProgram *assembly_program_new();

char *assembly_program_to_string(AssemblyProgram *program);
char *external_directive_to_string(AssemblyProgram *program);
char *global_directive_to_string(AssemblyProgram *program);
void assembly_program_free(AssemblyProgram *program);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_PROGRAM_H__