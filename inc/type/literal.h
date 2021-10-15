#ifndef LUVA_LITERAL_H
#define LUVA_LITERAL_H

typedef enum _LiteralType {
    LITERAL_NUMBER,
    LITERAL_STRING,
    LITERAL_IDENTIFIER
} LiteralType;

typedef struct Literal {
    LiteralType type;
    char *value;
} Literal_T;

#endif