#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__

typedef enum _AssemblyDataSectionTypes {
    DATA_TYPE_DB,
    DATA_TYPE_DW,
    DATA_TYPE_DD,
    DATA_TYPE_DQ,
    DATA_TYPE_EQU
} AssemblyDataSectionTypes;

typedef enum _AssemblyBssSectionTypes {
    BSS_TYPE_RESB,
    BSS_TYPE_RESW,
    BSS_TYPE_RESD,
    BSS_TYPE_RESQ
} AssemblyBssSectionTypes;

extern const char *_AssemblyDataSectionTypes_names[];
extern const char *_AssemblyBssSectionTypes_names[];

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__