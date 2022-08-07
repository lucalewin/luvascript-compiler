#include <generation/arch/x86-64/assembly/registers.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _RegisterLayout {
	RegisterInfo *registers[REGISTER_COUNT];
};

struct _RegisterInfo {
	unsigned int is_empty : 1;
    size_t value_type;
    size_t bytes : 1;
    char *value;
};

enum _RegisterInfoValueType {
	REGISTER_INFO_VALUE_TYPE_NONE,
	REGISTER_INFO_VALUE_TYPE_VARIABLE,
	REGISTER_INFO_VALUE_TYPE_VALUE
};

RegisterLayout* register_layout_new() {
    RegisterLayout *layout = calloc(1, sizeof(RegisterLayout));
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		layout->registers[i] = calloc(1, sizeof(RegisterInfo));
		layout->registers[i]->is_empty = 1;
		layout->registers[i]->value_type = REGISTER_INFO_VALUE_TYPE_NONE;
		layout->registers[i]->value = NULL;
	}
    return layout;
}

void register_setValue(RegisterLayout *layout, Register reg, size_t bytes, char *value) {
	layout->registers[reg]->is_empty = 0;
	layout->registers[reg]->value_type = REGISTER_INFO_VALUE_TYPE_VALUE;
	layout->registers[reg]->bytes = bytes;
	layout->registers[reg]->value = value;
}

void register_setVariable(RegisterLayout *layout, Register reg, size_t bytes, char *var_name) {
	layout->registers[reg]->is_empty = 0;
	layout->registers[reg]->value_type = REGISTER_INFO_VALUE_TYPE_VARIABLE;
	layout->registers[reg]->bytes = bytes;
	layout->registers[reg]->value = var_name;
}

char *register_getValue(RegisterLayout *layout, Register reg) {
	return layout->registers[reg]->value;
}

char *register_getValueType(RegisterLayout *layout, Register reg) {
	switch (layout->registers[reg]->value_type) {
		case REGISTER_INFO_VALUE_TYPE_NONE:
			return "none";
		case REGISTER_INFO_VALUE_TYPE_VARIABLE:
			return "variable";
		case REGISTER_INFO_VALUE_TYPE_VALUE:
			return "value";
	}
	return "unknown";
}

void register_clear(RegisterLayout *layout, Register reg) {
	layout->registers[reg]->is_empty = 1;
	layout->registers[reg]->value_type = REGISTER_INFO_VALUE_TYPE_NONE;
	layout->registers[reg]->bytes = 0;
	layout->registers[reg]->value = NULL;
}

void register_clearAll(RegisterLayout *layout) {
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		register_clear(layout, i);
	}
}

RegisterInfo *register_layout_getRegisterInfo(RegisterLayout *layout, Register reg) {
    // if (reg >= 0 && reg < REGISTER_COUNT) {
	// 	return layout->registers[reg];
	// }
	// return NULL;
	return layout->registers[reg];
}

int registerinfo_isEmpty(RegisterInfo *info) {
	return info->is_empty;
}

int register_isEmpty(RegisterLayout *layout, Register reg) {
	return layout->registers[reg]->is_empty;
}

Register register_getEmpty(RegisterLayout *layout) {
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (layout->registers[i]->is_empty) {
			return i;
		}
	}
	return -1;
}

void register_layout_free(RegisterLayout *layout) {
    if (layout == NULL) {
        return;
    }

	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (layout->registers[i]->value != NULL) {
			free(layout->registers[i]->value);
		}
		free(layout->registers[i]);
	}

    free(layout);
}

Register getEmptyRegister(RegisterLayout *layout) {
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (layout->registers[i]->is_empty) {
			return (Register)i;
		}
	}
	return -1;
}


int register_containsVariable(RegisterLayout *layout, char *var_name) {
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (layout->registers[i]->value_type == REGISTER_INFO_VALUE_TYPE_VARIABLE &&
					strcmp(layout->registers[i]->value, var_name) == 0) {
						// printf("%s\n", layout->registers[i]->value);
			return 1;
		}
	}
	return 0;
}

Register register_getVariable(RegisterLayout *layout, char *var_name) {
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (layout->registers[i]->value_type == REGISTER_INFO_VALUE_TYPE_VARIABLE &&
					strcmp(layout->registers[i]->value, var_name) == 0) {
			return (Register)i;
		}
	}
	return -1;
}


char *register_toString(Register reg, int opCodeSize) {
	switch (opCodeSize) {
		case 1:
			switch (reg) {
				case REGISTER_RAX:
					return "al";
				case REGISTER_RBX:
					return "bl";
				case REGISTER_RCX:
					return "cl";
				case REGISTER_RDX:
					return "dl";
				case REGISTER_RSI:
					return "sil";
				case REGISTER_RDI:
					return "dil";
				case REGISTER_RBP:
					return "bpl";
				case REGISTER_RSP:
					return "spl";
				case REGISTER_RIP:
					return "eip";
				case REGISTER_R8:
					return "r8b";
				case REGISTER_R9:
					return "r9b";
				case REGISTER_R10:
					return "r10b";
				case REGISTER_R11:
					return "r11b";
				case REGISTER_R12:
					return "r12b";
				case REGISTER_R13:
					return "r13b";
				case REGISTER_R14:
					return "r14b";
				case REGISTER_R15:
					return "r15b";
				case REGISTER_XMM0:
					return "xmm0";
				case REGISTER_XMM1:
					return "xmm1";
				case REGISTER_XMM2:
					return "xmm2";
				case REGISTER_XMM3:
					return "xmm3";
				case REGISTER_XMM4:
					return "xmm4";
				case REGISTER_XMM5:
					return "xmm5";
				case REGISTER_XMM6:
					return "xmm6";
				case REGISTER_XMM7:
					return "xmm7";
				case REGISTER_XMM8:
					return "xmm8";
				case REGISTER_XMM9:
					return "xmm9";
				case REGISTER_XMM10:
					return "xmm10";
				case REGISTER_XMM11:
					return "xmm11";
				case REGISTER_XMM12:
					return "xmm12";
				case REGISTER_XMM13:
					return "xmm13";
				case REGISTER_XMM14:
					return "xmm14";
				case REGISTER_XMM15:
					return "xmm15";
				default:
					return NULL;
			}
			break;
		case 2:
			switch (reg) {
				case REGISTER_RAX:
					return "ax";
				case REGISTER_RBX:
					return "bx";
				case REGISTER_RCX:
					return "cx";
				case REGISTER_RDX:
					return "dx";
				case REGISTER_RSI:
					return "si";
				case REGISTER_RDI:
					return "di";
				case REGISTER_RBP:
					return "bp";
				case REGISTER_RSP:
					return "sp";
				case REGISTER_RIP:
					return "ip";
				case REGISTER_R8:
					return "r8w";
				case REGISTER_R9:
					return "r9w";
				case REGISTER_R10:
					return "r10w";
				case REGISTER_R11:
					return "r11w";
				case REGISTER_R12:
					return "r12w";
				case REGISTER_R13:
					return "r13w";
				case REGISTER_R14:
					return "r14w";
				case REGISTER_R15:
					return "r15w";
				case REGISTER_XMM0:
					return "xmm0";
				case REGISTER_XMM1:
					return "xmm1";
				case REGISTER_XMM2:
					return "xmm2";
				case REGISTER_XMM3:
					return "xmm3";
				case REGISTER_XMM4:
					return "xmm4";
				case REGISTER_XMM5:
					return "xmm5";
				case REGISTER_XMM6:
					return "xmm6";
				case REGISTER_XMM7:
					return "xmm7";
				case REGISTER_XMM8:
					return "xmm8";
				case REGISTER_XMM9:
					return "xmm9";
				case REGISTER_XMM10:
					return "xmm10";
				case REGISTER_XMM11:
					return "xmm11";
				case REGISTER_XMM12:
					return "xmm12";
				case REGISTER_XMM13:
					return "xmm13";
				case REGISTER_XMM14:
					return "xmm14";
				case REGISTER_XMM15:
					return "xmm15";
				default:
					return NULL;
			}
		case 4:
			switch (reg) {
				case REGISTER_RAX:
					return "eax";
				case REGISTER_RBX:
					return "ebx";
				case REGISTER_RCX:
					return "ecx";
				case REGISTER_RDX:
					return "edx";
				case REGISTER_RSI:
					return "esi";
				case REGISTER_RDI:
					return "edi";
				case REGISTER_RBP:
					return "ebp";
				case REGISTER_RSP:
					return "esp";
				case REGISTER_RIP:
					return "eip";
				case REGISTER_R8:
					return "r8d";
				case REGISTER_R9:
					return "r9d";
				case REGISTER_R10:
					return "r10d";
				case REGISTER_R11:
					return "r11d";
				case REGISTER_R12:
					return "r12d";
				case REGISTER_R13:
					return "r13d";
				case REGISTER_R14:
					return "r14d";
				case REGISTER_R15:
					return "r15d";
				case REGISTER_XMM0:
					return "xmm0";
				case REGISTER_XMM1:
					return "xmm1";
				case REGISTER_XMM2:
					return "xmm2";
				case REGISTER_XMM3:
					return "xmm3";
				case REGISTER_XMM4:
					return "xmm4";
				case REGISTER_XMM5:
					return "xmm5";
				case REGISTER_XMM6:
					return "xmm6";
				case REGISTER_XMM7:
					return "xmm7";
				case REGISTER_XMM8:
					return "xmm8";
				case REGISTER_XMM9:
					return "xmm9";
				case REGISTER_XMM10:
					return "xmm10";
				case REGISTER_XMM11:
					return "xmm11";
				case REGISTER_XMM12:
					return "xmm12";
				case REGISTER_XMM13:
					return "xmm13";
				case REGISTER_XMM14:
					return "xmm14";
				case REGISTER_XMM15:
					return "xmm15";
				default:
					return NULL;
			}
		case 8:
			switch (reg) {
				case REGISTER_RAX:
					return "rax";
				case REGISTER_RBX:
					return "rbx";
				case REGISTER_RCX:
					return "rcx";
				case REGISTER_RDX:
					return "rdx";
				case REGISTER_RSI:
					return "rsi";
				case REGISTER_RDI:
					return "rdi";
				case REGISTER_RBP:
					return "rbp";
				case REGISTER_RSP:
					return "rsp";
				case REGISTER_RIP:
					return "rip";
				case REGISTER_R8:
					return "r8";
				case REGISTER_R9:
					return "r9";
				case REGISTER_R10:
					return "r10";
				case REGISTER_R11:
					return "r11";
				case REGISTER_R12:
					return "r12";
				case REGISTER_R13:
					return "r13";
				case REGISTER_R14:
					return "r14";
				case REGISTER_R15:
					return "r15";
				case REGISTER_XMM0:
					return "xmm0";
				case REGISTER_XMM1:
					return "xmm1";
				case REGISTER_XMM2:
					return "xmm2";
				case REGISTER_XMM3:
					return "xmm3";
				case REGISTER_XMM4:
					return "xmm4";
				case REGISTER_XMM5:
					return "xmm5";
				case REGISTER_XMM6:
					return "xmm6";
				case REGISTER_XMM7:
					return "xmm7";
				case REGISTER_XMM8:
					return "xmm8";
				case REGISTER_XMM9:
					return "xmm9";
				case REGISTER_XMM10:
					return "xmm10";
				case REGISTER_XMM11:
					return "xmm11";
				case REGISTER_XMM12:
					return "xmm12";
				case REGISTER_XMM13:
					return "xmm13";
				case REGISTER_XMM14:
					return "xmm14";
				case REGISTER_XMM15:
					return "xmm15";
				default:
					return NULL;
			}
		default:
			return NULL;
	}
}
