#include <generation/arch/x86-64/assembly/section.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/logging/logger.h>
#include <util/util.h>
#include <generation/arch/x86-64/assembly/instruction.h>
#include <generation/arch/x86-64/assembly/datatypes.h>

// partly implementation of assembly/section.h (only the text part) 

TextSection *text_section_new() {
    TextSection *section = calloc(1, sizeof(TextSection));
    section->instructions = arraylist_create();
    return section;
}

void text_section_add_label(TextSection *section, char *label) {
    AssemblyInstruction *inst = assembly_instruction_new(label, NULL, NULL, 0, NULL);
    arraylist_add(section->instructions, inst);
}

void text_section_add_instruction(TextSection *section, char *instruction, char *operands[], int count) {
    AssemblyInstruction *inst = assembly_instruction_new(NULL, instruction, operands, count, NULL);
    arraylist_add(section->instructions, inst);
}

char *text_section_to_string(TextSection *section) {
    size_t buffer_size = 0;

    // calculate length
    buffer_size += 14; // 'section .text\n'

    // '\t' + mnemonic + ' ' + (operands + ', ')* + '\n'
    for (size_t i = 0; i < arraylist_size(section->instructions); i++) {
        AssemblyInstruction *inst = arraylist_get(section->instructions, i);
        
        if (inst->label != NULL) {
            buffer_size += strlen(inst->label) + 1; // label + ':'
        }

        if (inst->mnemonic != NULL) {
            buffer_size += 1 + strlen(inst->mnemonic); // '\t' + mnemonic
        }

        for (int j = 0; j < inst->operand_count; j++) {
            buffer_size += 1 + strlen(inst->operands[j]); // ' ' + operand

            if (j < inst->operand_count - 1) {
                buffer_size += 1; // ','
            }
        }

        if (inst->comment != NULL) {
            buffer_size += 3 + strlen(inst->comment); // '\t' + ';' + ' ' + comment
        }

        buffer_size += 1; // '\n'
    }

    // copy all strings into buffer
    char *buffer = calloc(buffer_size + 1, sizeof(char));

    strcpy(buffer, "section .text\n");

    for (size_t i = 0; i < arraylist_size(section->instructions); i++) {
        AssemblyInstruction *inst = arraylist_get(section->instructions, i);

        if (inst->label != NULL) {
            strcat(buffer, inst->label);
            strcat(buffer, ":");
        }

        if (inst->mnemonic != NULL) {
            strcat(buffer, "\t");
            strcat(buffer, inst->mnemonic);
        }

        for (int j = 0; j < inst->operand_count; j++) {
            strcat(buffer, " ");
            strcat(buffer, inst->operands[j]);

            if (j < inst->operand_count - 1) {
                strcat(buffer, ",");
            }
        }

        if (inst->comment != NULL) {
            strcat(buffer, "\t ");
            strcat(buffer, inst->comment);
        }

        strcat(buffer, "\n");
    }
    // TODO: copy strings

    return buffer;
}

void text_section_free(TextSection *section) {
    if (section == NULL) {
        return;
    }

    for (size_t i = 0; i < arraylist_size(section->instructions); i++) {
        AssemblyInstruction *instruction = arraylist_get(section->instructions, i);
        assembly_instruction_free(instruction);
    }

    arraylist_free(section->instructions);

    free(section);
}

// implemention of assembly/instruction.h

AssemblyInstruction *assembly_instruction_new(char *label, char *mnemonic, char *operands[], int operand_count, char *comment) {
    AssemblyInstruction *instruction = calloc(1, sizeof(AssemblyInstruction));
    
    instruction->label = strdup(label);

    // this should always be a statically allocated string
    instruction->mnemonic = mnemonic;
    
    // copy operands into instruction->operands
    instruction->operand_count = operand_count;
    instruction->operands = copy_all(operands, operand_count);

    // this should always be a statically allocated string
    instruction->comment = comment;
    
    return instruction;
}

void assembly_instruction_free(AssemblyInstruction *instruction) {
    if (instruction == NULL) {
        return;
    }

    free(instruction->label);
    
    // Don't free the mnemonic, it's a constant
    // free(instruction->mnemonic);

    // Don't free the comment, it's a constant
    // free(instruction->comment);

    for (size_t i = 0; i < instruction->operand_count; i++) {
        free(instruction->operands[i]);
    }

    free(instruction);
}

