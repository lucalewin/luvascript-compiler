#include <assembly/registers.h>

#include <stdio.h>
#include <stdlib.h>

struct _RegisterLayout {

};

struct _RegisterInfo {
    size_t value_type;
    size_t bytes;
    char *value;
};

RegisterLayout* register_layout_new() {
    RegisterLayout *layout = calloc(1, sizeof(RegisterLayout));
    return layout;
}

void register_layout_set_value(RegisterLayout layout, Register reg, size_t bytes, char *value) {

}

void register_layout_set_var(RegisterLayout layout, Register reg, size_t bytes, char *var_name) {

}

RegisterInfo *register_layout_get(RegisterLayout *layout, Register reg) {
    return NULL;
}

void register_layout_free(RegisterLayout *layout) {
    if (layout == NULL) {
        return;
    }

    free(layout);
}

char *getRegisterWithOpCodeSize(Register reg, int opCodeSize) {
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
