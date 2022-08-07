#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__

typedef enum _AssemblyDataSectionTypes {
    DATA_TYPE_EQU = 0,
    DATA_TYPE_DB = 1,
    DATA_TYPE_DW = 2,
    DATA_TYPE_DD = 4,
    DATA_TYPE_DQ = 8,
} AssemblyDataSectionTypes;

typedef enum _AssemblyBssSectionTypes {
    BSS_TYPE_RESB = 1,
    BSS_TYPE_RESW = 2,
    BSS_TYPE_RESD = 4,
    BSS_TYPE_RESQ = 8
} AssemblyBssSectionTypes;

typedef enum _DatatypeDirective {
	DATATYPE_DIRECTIVE_BYTE = 1,
	DATATYPE_DIRECTIVE_WORD = 2,
	DATATYPE_DIRECTIVE_DWORD = 4,
	DATATYPE_DIRECTIVE_QWORD = 8
} DatatypeDirective;

extern const char *_AssemblyDataSectionTypes_names[];
extern const char *_AssemblyBssSectionTypes_names[];
extern const char *_AssemblyDataType_directives[];

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_DATATYPES_H__