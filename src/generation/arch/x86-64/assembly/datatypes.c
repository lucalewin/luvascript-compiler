#include <generation/arch/x86-64/assembly/datatypes.h>

const char *_AssemblyDataSectionTypes_names[] = {
    [DATA_TYPE_EQU] = "equ",
    [DATA_TYPE_DB] = "db",
    [DATA_TYPE_DW] = "dw",
    [DATA_TYPE_DD] = "dd",
    [DATA_TYPE_DQ] = "dq",
};

const char *_AssemblyBssSectionTypes_names[] = {
    [BSS_TYPE_RESB] = "resb",
    [BSS_TYPE_RESW] = "resw",
    [BSS_TYPE_RESD] = "resd",
    [BSS_TYPE_RESQ] = "resq"
};

const char *_AssemblyDataType_directives[] = {
    [DATATYPE_DIRECTIVE_BYTE] = "byte",
    [DATATYPE_DIRECTIVE_WORD] = "word",
    [DATATYPE_DIRECTIVE_DWORD] = "dword",
    [DATATYPE_DIRECTIVE_QWORD] = "qword"
};
