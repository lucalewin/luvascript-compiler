#ifndef LUVA_AST_H
#define LUVA_AST_H

#include <statement.h>

typedef struct _ast AST;

struct _ast {
    Statement *statement;
};

#endif // LUVA_AST_H