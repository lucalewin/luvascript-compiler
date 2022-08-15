#ifndef LUVA_LITERAL_H
#define LUVA_LITERAL_H

#include <types/datatype.h>

typedef enum _LiteralType {
    LITERAL_NUMBER,
    LITERAL_STRING,
	LITERAL_CHARACTER,
    LITERAL_IDENTIFIER,
	LITERAL_BOOLEAN
} LiteralType;

typedef struct _literal {
    LiteralType type;
    char *value;
    DataType *datatype;
} Literal;

extern const char *LITERAL_TYPES[];

Literal *literal_create(LiteralType type, char *value);
void literal_free(Literal *literal);

#endif