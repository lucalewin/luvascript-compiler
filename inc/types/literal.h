#ifndef LUVA_LITERAL_H
#define LUVA_LITERAL_H

typedef enum _LiteralType {
    LITERAL_NUMBER,
    LITERAL_STRING,
	LITERAL_CHAR,
    LITERAL_IDENTIFIER,
	LITERAL_BOOLEAN
} LiteralType;

typedef struct Literal {
    LiteralType type;
    char *value;
} Literal_T;

extern const char *LITERAL_TYPES[];

void literal_free(Literal_T *literal);

#endif