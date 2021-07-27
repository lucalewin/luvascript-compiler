#ifndef LUVA_x86_64_ASSEMBLY
#define LUVA_x86_64_ASSEMBLY

// -------------------------------

typedef enum { data, rodata, bss, text } SectionType;
typedef enum { db, dw, dd, dq, ddq, dt } VarType;
typedef enum { } InstType;

// -------------------------------

typedef struct {
    // sections
    Section *sections;
    int sectionCount = 0;
} Assembly;

typedef struct {
    // type of section
    SectionType type;
    // array of functions
    Func *functions;
    int funcCount = 0;
    // array of constants
    Const *constants;
    int constCount = 0;
    // array of variables
    Var *vars;
    int varCount = 0;
} Section;

typedef struct {
    Block *blocks;
    int blocksCount = 0;
} Func;

typedef struct {
    const char *name;
    void *value;
} Const;

typedef struct {
    const char *name;
    VarType type;
    void *value;
} Var;

// block of instructions
typedef struct {
    // label of the block
    const char *label;
    // list of instructions
    Inst *instructions;
    int instCount = 0;
} Block;

// instruction
typedef struct {
;
} Inst;

#endif // LUVA_x86_64_ASSEMBLY