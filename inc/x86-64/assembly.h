#ifndef LUVA_x86_64_ASSEMBLY
#define LUVA_x86_64_ASSEMBLY

// -------------------------------

typedef struct _assembly Assembly;
typedef struct _section Section;
typedef struct _function Function;
typedef struct _const Const;
typedef struct _var Var;
typedef struct _block Block;
typedef struct _instruction Instruction;

typedef enum { data, rodata, bss, text } SectionType;
typedef enum { db, dw, dd, dq, ddq, dt } VarType;
typedef enum { a, b, c, d, e, f, g, h  } InstType;

// -------------------------------

struct _assembly {
    // sections
    Section *sections;
    int sectionCount;
};

struct _section {
    // type of section
    SectionType type;
    // array of functions
    Function *functions;
    int funcCount;
    // array of constants
    Const *constants;
    int constCount;
    // array of variables
    Var *vars;
    int varCount;
};

struct _function {
    Block *blocks;
    int blocksCount;
};

struct _const {
    const char *name;
    void *value;
};

struct _var {
    const char *name;
    VarType type;
    void *value;
};

// block of instructions
struct _block {
    // label of the block
    const char *label;
    // list of instructions
    Instruction *instructions;
    int instCount;
};

// instruction
struct _instruction {
    InstType type;
};

#endif // LUVA_x86_64_ASSEMBLY