#include <assembly/stack.h>

#include <stdlib.h>

struct _StackLayout {
    int i;
};

StackLayout* stack_layout_new() {
    return NULL;
}

void stack_layout_free(StackLayout *stack_layout) {
    if (stack_layout == NULL) {
        return;
    }

    free(stack_layout);
}

void stack_push_register(StackLayout *stack_layout, Register reg, RegisterLayout *layout) {

}

void stack_push_variable(StackLayout *stack_layout, char *var_name) {

}

void stack_pop_register(StackLayout *stack_layout, Register reg, RegisterLayout *layout) {

}

void stack_pop_variable(StackLayout *stack_layout, char *var_name) {

}
