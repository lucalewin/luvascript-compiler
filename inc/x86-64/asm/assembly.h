#ifndef LUVA_x86_64_ASSEMBLY
#define LUVA_x86_64_ASSEMBLY

typedef struct _func Func;
typedef struct _var Var;

typedef struct _block Block;

struct _assembly {
    // array of functions
    Func *functions;
    // array of variables
    Var *vars;
};

struct _func {
    Block *blocks;
};

struct _var {

};

struct _block {

};

#endif // LUVA_x86_64_ASSEMBLY