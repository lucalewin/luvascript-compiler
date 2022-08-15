#ifndef __LUVA_NODES_NODE_H__
#define __LUVA_NODES_NODE_H__

typedef struct _node {
    char *filename;
    int line;
    int column;
} Node;

#endif // __LUVA_NODES_NODE_H__