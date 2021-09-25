#include <ast.h>

AST *ast_init() {
    AST *ast = calloc(1, sizeof(struct _ast));
    return ast;
}