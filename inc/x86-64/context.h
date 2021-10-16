#ifndef LUVA_x68_64_CONTEXT_H
#define LUVA_x68_64_CONTEXT_H

#include <util/arraylist.h>

typedef struct CONTEXT {
    ArrayList *global_variables;
    ArrayList *local_variables;
    ArrayList *functions;
} Context_t;

#endif // LUVA_x68_64_CONTEXT_H