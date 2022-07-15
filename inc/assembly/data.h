#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_DATA_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_DATA_H__

#include <assembly/datatypes.h>

typedef struct _assemblyDataVariable {
    char *label;
    AssemblyDataSectionTypes type;
    char *value;
} AssemblyDataVariable;

void assembly_data_variable_free(AssemblyDataVariable *variable);
AssemblyDataVariable *assembly_data_variable_new(char *label, AssemblyDataSectionTypes type, char *value);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_DATA_H__