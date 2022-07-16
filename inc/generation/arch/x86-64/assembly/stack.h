#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__

#include <generation/arch/x86-64/assembly/registers.h>

typedef struct _StackLayout StackLayout;

StackLayout* stack_layout_new();
void stack_layout_free(StackLayout *stack_layout);

void stack_push_register(StackLayout *stack_layout, Register reg, RegisterLayout *layout);
void stack_push_variable(StackLayout *stack_layout, char *var_name);

void stack_pop_register(StackLayout *stack_layout, Register reg, RegisterLayout *layout);
void stack_pop_variable(StackLayout *stack_layout, char *var_name);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__