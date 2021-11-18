#include <register.h>

char *getRegisterWithOpCodeSize(Register reg, int opCodeSize) {
	switch (opCodeSize) {
		case 1:
			switch (reg) {
				case REGISTER_EAX:
					return "al";
				case REGISTER_EBX:
					return "bl";
				case REGISTER_ECX:
					return "cl";
				case REGISTER_EDX:
					return "dl";
				case REGISTER_ESI:
					return "sil";
				case REGISTER_EDI:
					return "dil";
				case REGISTER_EBP:
					return "bpl";
				case REGISTER_ESP:
					return "spl";
				case REGISTER_EIP:
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
				case REGISTER_EAX:
					return "ax";
				case REGISTER_EBX:
					return "bx";
				case REGISTER_ECX:
					return "cx";
				case REGISTER_EDX:
					return "dx";
				case REGISTER_ESI:
					return "si";
				case REGISTER_EDI:
					return "di";
				case REGISTER_EBP:
					return "bp";
				case REGISTER_ESP:
					return "sp";
				case REGISTER_EIP:
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
				case REGISTER_EAX:
					return "eax";
				case REGISTER_EBX:
					return "ebx";
				case REGISTER_ECX:
					return "ecx";
				case REGISTER_EDX:
					return "edx";
				case REGISTER_ESI:
					return "esi";
				case REGISTER_EDI:
					return "edi";
				case REGISTER_EBP:
					return "ebp";
				case REGISTER_ESP:
					return "esp";
				case REGISTER_EIP:
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
				case REGISTER_EAX:
					return "rax";
				case REGISTER_EBX:
					return "rbx";
				case REGISTER_ECX:
					return "rcx";
				case REGISTER_EDX:
					return "rdx";
				case REGISTER_ESI:
					return "rsi";
				case REGISTER_EDI:
					return "rdi";
				case REGISTER_EBP:
					return "rbp";
				case REGISTER_ESP:
					return "rsp";
				case REGISTER_EIP:
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