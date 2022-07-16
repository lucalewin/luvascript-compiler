#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_INSTRUCTION_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_INSTRUCTION_H__

typedef struct _AssemblyInstruction {
    char *label;
    const char *mnemonic;
    char **operands;
    int operand_count;
    const char *comment;
} AssemblyInstruction;

AssemblyInstruction *assembly_instruction_new(char *label, char *mnemonic, char **operands, int operands_count, char *comment);
void assembly_instruction_free(AssemblyInstruction *instruction);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_INSTRUCTION_H__