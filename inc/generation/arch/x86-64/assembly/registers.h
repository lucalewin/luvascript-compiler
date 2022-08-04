#ifndef __LUVA_X86_64_COMPILER_REGISTERS_H__
#define __LUVA_X86_64_COMPILER_REGISTERS_H__

#include <stdio.h>

typedef enum _register {
    REGISTER_RAX,
    REGISTER_RBX,
    REGISTER_RCX,
    REGISTER_RDX,
    REGISTER_RDI,
    REGISTER_RSI,
    REGISTER_RBP,
    REGISTER_RSP,
    REGISTER_RIP,
    REGISTER_R8,
    REGISTER_R9,
    REGISTER_R10,
    REGISTER_R11,
    REGISTER_R12,
    REGISTER_R13,
    REGISTER_R14,
    REGISTER_R15,
    REGISTER_XMM0,
    REGISTER_XMM1,
    REGISTER_XMM2,
    REGISTER_XMM3,
    REGISTER_XMM4,
    REGISTER_XMM5,
    REGISTER_XMM6,
    REGISTER_XMM7,
    REGISTER_XMM8,
    REGISTER_XMM9,
    REGISTER_XMM10,
    REGISTER_XMM11,
    REGISTER_XMM12,
    REGISTER_XMM13,
    REGISTER_XMM14,
    REGISTER_XMM15
} Register;

#ifndef REGISTER_COUNT
    #define REGISTER_COUNT REGISTER_XMM15 + 1
#endif

typedef struct _RegisterLayout RegisterLayout;
typedef struct _RegisterInfo RegisterInfo;

RegisterLayout* register_layout_new();

void register_setValue(RegisterLayout *layout, Register reg, size_t bytes, char *value);
void register_setVariable(RegisterLayout *layout, Register reg, size_t bytes, char *var_name);

char *register_getValue(RegisterLayout *layout, Register reg);
char *register_getValueType(RegisterLayout *layout, Register reg);

RegisterInfo *register_layout_getRegisterInfo(RegisterLayout *layout, Register reg);

void register_layout_free(RegisterLayout *layout);

int registerinfo_isEmpty(RegisterInfo *info);
int register_isEmpty(RegisterLayout *layout, Register reg);
Register register_getEmpty(RegisterLayout *layout);

void register_clear(RegisterLayout *layout, Register reg);
void register_clearAll(RegisterLayout *layout);

Register getEmptyRegister(RegisterLayout *layout);
char *register_toString(Register reg, int opCodeSize);

int register_containsVariable(RegisterLayout *layout, char *var_name);
Register register_getVariable(RegisterLayout *layout, char *var_name);

#endif // __LUVA_X86_64_COMPILER_REGISTERS_H__