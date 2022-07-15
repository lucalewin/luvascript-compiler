#include <assembly/program.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assembly/section.h>

AssemblyProgram *assembly_program_new() {
    AssemblyProgram *assembly = calloc(1, sizeof(AssemblyProgram));
    assembly->global_directives = arraylist_create();
    assembly->extern_directives = arraylist_create();
    assembly->data = data_section_new();
    assembly->bss = bss_section_new();
    assembly->text = text_section_new();
    return assembly;
}

char *assembly_program_to_string(AssemblyProgram *program) {
    char *global_directives = global_directive_to_string(program);
    char *extern_directives = external_directive_to_string(program);
    char *data_section      = data_section_to_string(program->data);
    char *bss_section       = bss_section_to_string(program->bss);
    char *text_section      = text_section_to_string(program->text);

    size_t buffer_size = strlen(global_directives)
                       + strlen(extern_directives)
                       + strlen(data_section)
                       + strlen(bss_section)
                       + strlen(text_section)
                       + 1; // null terminated string

    char *buffer = calloc(buffer_size, sizeof(char));

    strcpy(buffer, global_directives);
    strcat(buffer, extern_directives);
    strcat(buffer, data_section);
    strcat(buffer, bss_section);
    strcat(buffer, text_section);

    free(data_section);
    free(bss_section);
    free(text_section);

    return buffer;
}

char *global_directive_to_string(AssemblyProgram *program) {
    size_t buffer_size = 0;

    for (size_t i = 0; i < arraylist_size(program->global_directives); i++) {
        buffer_size += 7 + strlen(arraylist_get(program->global_directives, i)) + 1; // 'global ' + directive + '\n'
    }

    char *buffer = calloc(buffer_size + 1, sizeof(char));

    for (size_t i = 0; i < arraylist_size(program->global_directives); i++) {
        strcat(buffer, "global ");
        strcat(buffer, arraylist_get(program->global_directives, i));
        strcat(buffer, "\n");
    }

    return buffer;
}

char *external_directive_to_string(AssemblyProgram *program) {
    size_t buffer_size = 0;

    for (size_t i = 0; i < arraylist_size(program->extern_directives); i++) {
        buffer_size += 7 + strlen(arraylist_get(program->extern_directives, i)) + 1;
    }

    char *buffer = calloc(buffer_size + 1, sizeof(char));

    for (size_t i = 0; i < arraylist_size(program->extern_directives); i++) {
        strcat(buffer, "extern ");
        strcat(buffer, arraylist_get(program->extern_directives, i));
        strcat(buffer, "\n");
    }

    return buffer;
}

void assembly_program_free(AssemblyProgram *program) {
    if (program == NULL) {
        return;
    }

    // free list of global directives
    if (program->global_directives != NULL) {
        for (size_t i = 0; i < arraylist_size(program->global_directives); i++) {
            char *global_directive = arraylist_get(program->global_directives, i);
            free(global_directive);
        }
        arraylist_free(program->global_directives);
    }

    // free list of extern directives
    if (program->extern_directives != NULL) {
        for (size_t i = 0; i < arraylist_size(program->extern_directives); i++) {
            char *extern_directive = arraylist_get(program->extern_directives, i);
            free(extern_directive);
        }
        arraylist_free(program->extern_directives);
    }

    data_section_free(program->data);
    // rodata_section_free(program->rodata);
    bss_section_free(program->bss);
    text_section_free(program->text);
    free(program);
}
