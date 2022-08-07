#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__

#include <generation/arch/x86-64/assembly/registers.h>

typedef struct _StackLayout StackLayout;
typedef struct _StackItem StackItem;

StackLayout* stack_layout_new();
void freeStackLayout(StackLayout *stack_layout);

// new
size_t stack_getSize(StackLayout *stack_layout);

void stack_allocate(StackLayout *stack_layout, size_t size);
void stack_deallocate(StackLayout *stack_layout, size_t size);

void stack_pushRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout);
void stack_popRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout);

void stack_pushVariable(StackLayout *stack_layout, char *var_name, size_t size);
void stack_popVariable(StackLayout *stack_layout, char *var_name, size_t size);

void stack_setRegister(StackLayout *stack_layout, Register reg, size_t size, RegisterLayout *reg_layout, size_t rbp_offset);
void stack_setVariable(StackLayout *stack_layout, char *var_name, size_t size, size_t rbp_offset);

size_t stack_getVariableOffset(StackLayout *stack_layout, char *var_name);
size_t stack_getItemSize(StackLayout *stack_layout, size_t item_rbp_offset);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_STACK_H__