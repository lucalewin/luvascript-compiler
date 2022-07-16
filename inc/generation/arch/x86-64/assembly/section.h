#ifndef __LUVA_X86_64_COMPILER_ASSEMBLY_SECTION_H__
#define __LUVA_X86_64_COMPILER_ASSEMBLY_SECTION_H__

#include <stdio.h>

#include <util/arraylist.h>
#include <generation/arch/x86-64/assembly/datatypes.h>

typedef struct _DataSection {
    ArrayList *data; // array list of AssemblyDataVariable*
} DataSection;

// typedef struct _RodataSection {
    
// } RodataSection;

typedef struct _BssSection {
    
} BssSection;

typedef struct _TextSection {
    ArrayList *instructions; // array of AssemblyInstruction*
} TextSection;


// Data Section
DataSection *data_section_new();
char *data_section_to_string(DataSection *section);
void data_section_declare_variable(DataSection *section, char *label, AssemblyDataSectionTypes type, char *value);
void data_section_free(DataSection *section);

// Rodata Section
// RodataSection *rodata_section_new();
// char *rodata_section_to_string(RodataSection *section);
// void rodata_section_add_label(RodataSection *section, const char *label);
// void rodata_section_add_constant(RodataSection *section, char *label, char *value);
// void rodata_section_free(RodataSection *section);

// Bss Section
BssSection *bss_section_new();
char *bss_section_to_string(BssSection *section);
void bss_section_add_label(BssSection *section, const char *label);
void bss_section_declare_space(BssSection *section, char *label, AssemblyBssSectionTypes type, size_t count);
void bss_section_free(BssSection *section);

// Text Section
TextSection *text_section_new();
char *text_section_to_string(TextSection *section);
void text_section_add_label(TextSection *section, char *label);
void text_section_add_instruction(TextSection *section, char *instruction, char *operands[], int count);
void text_section_free(TextSection *section);

#endif // __LUVA_X86_64_COMPILER_ASSEMBLY_SECTION_H__