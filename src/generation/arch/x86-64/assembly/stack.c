#include <generation/arch/x86-64/assembly/stack.h>

#include <stdlib.h>
#include <string.h>

#include <util/util.h>
#include <util/arraylist.h>

struct _StackLayout {
    size_t size;
    ArrayList *items; // List of StackItem*
};

struct _StackItem {
    size_t size;
    size_t rbp_offset;
    enum { STACK_ITEM_REGISTER, STACK_ITEM_VARIABLE } type;
    char *name;
};

StackLayout* stack_layout_new() {
    StackLayout *stack_layout = malloc(sizeof(StackLayout));
    stack_layout->size = 0;
    stack_layout->items = arraylist_create();
    return stack_layout;
}

void freeStackLayout(StackLayout *stack_layout) {
    if (stack_layout == NULL) {
        return;
    }

    if (stack_layout->items != NULL) {
        for (int i = 0; i < arraylist_size(stack_layout->items); i++) {
            StackItem *item = arraylist_get(stack_layout->items, i);
            free(item->name);
            free(item);
        }
        arraylist_free(stack_layout->items);
    }

    free(stack_layout);
}

size_t stack_getSize(StackLayout *stack_layout) {
    if (stack_layout == NULL) {
        return -1;
    }

    return stack_layout->size;
}

void stack_allocate(StackLayout *stack_layout, size_t size) {
    if (stack_layout == NULL) {
        return;
    }

    stack_layout->size += size;
}

void stack_deallocate(StackLayout *stack_layout, size_t size) {
    if (stack_layout == NULL) {
        return;
    }

    stack_layout->size -= size;
}

// ----------------------------------------------------------------

void stack_pushRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout) {
    if (stack_layout == NULL) {
        return;
    }

    StackItem *item = malloc(sizeof(StackItem));
    item->size = size;
    item->rbp_offset = stack_layout->size;
    item->type = STACK_ITEM_REGISTER;
    item->name = NULL;
    arraylist_add(stack_layout->items, item);
    stack_layout->size += size;
}

void stack_popRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout) {

}

// ----------------------------------------------------------------

void stack_pushVariable(StackLayout *stack_layout, char *var_name, size_t size) {
    if (stack_layout == NULL) {
        return;
    }

    StackItem *item = malloc(sizeof(StackItem));
    item->size = size;
    item->rbp_offset = stack_layout->size;
    item->type = STACK_ITEM_VARIABLE;
    item->name = strdup(var_name);
    arraylist_add(stack_layout->items, item);
    stack_layout->size += size;
}

void stack_popVariable(StackLayout *stack_layout, char *var_name, size_t size) {

}

// ----------------------------------------------------------------

void stack_setRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout, size_t rbp_offset) {

}

void stack_setVariable(StackLayout *stack_layout, char *var_name, size_t size, size_t rbp_offset) {

}

// ----------------------------------------------------------------

size_t stack_getVariableOffset(StackLayout *stack_layout, char *var_name) {
    if (stack_layout == NULL) {
        return -1;
    }

    for (int i = 0; i < arraylist_size(stack_layout->items); i++) {
        StackItem *item = arraylist_get(stack_layout->items, i);
        if (item->type == STACK_ITEM_VARIABLE && strcmp(item->name, var_name) == 0) {
            return item->rbp_offset;
        }
    }

    printf("Error: stack_getVariableOffset: variable %s not found\n", var_name);

    return -1;
}


size_t stack_getItemSize(StackLayout *stack_layout, size_t item_rbp_offset) {
    if (stack_layout == NULL) {
        return -1;
    }

    for (int i = 0; i < arraylist_size(stack_layout->items); i++) {
        StackItem *item = arraylist_get(stack_layout->items, i);
        if (item->rbp_offset == item_rbp_offset) {
            return item->size;
        }
    }

    printf("Error: stack_getItemSize: item at rbp_offset %zu not found\n", item_rbp_offset);

    return -1;
}
