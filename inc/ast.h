#ifndef LUVA_AST_H
#define LUVA_AST_H

#include <arraylist.h>

typedef struct _ast AST;

struct _ast {
    ArrayList *functions;
    ArrayList *global_var_decls;
};

#endif // LUVA_AST_H