#include <parsing/nodes/literal.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/util.h>

Literal *literal_create(LiteralType type, char *value) {
    Literal *literal = malloc(sizeof(Literal));
    literal->type = type;
    literal->value = strdup(value);
    literal->datatype = NULL;
    return literal;
}
