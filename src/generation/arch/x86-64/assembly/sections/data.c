#include <assembly/section.h>
#include <assembly/data.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>
#include <util/util.h>
#include <assembly/data.h>
#include <assembly/datatypes.h>

// partly implementation of assembly/section.h (only the data part) 

DataSection *data_section_new() {
    DataSection *section = calloc(1, sizeof(DataSection));
    section-> data = arraylist_create();
    return section;
}

void data_section_declare_variable(DataSection *section, char *label, AssemblyDataSectionTypes type, char *value) {
    // log_debug("Declaring variable '%s' of type '%s' with value '%s'", label, _AssemblyDataSectionTypes_names[type], value);
    arraylist_add(section->data, assembly_data_variable_new(label, type, value));
}

char *data_section_to_string(DataSection *section) {
    size_t buffer_length = 14;

    for (size_t i = 0; i < arraylist_size(section->data); i++) {
        AssemblyDataVariable *variable = arraylist_get(section->data, i);
        buffer_length += strlen(variable->label) + 2
                      +  strlen(_AssemblyDataSectionTypes_names[variable->type]) + 1
                      +  strlen(variable->value) + 1;
    }


    char *str = calloc(buffer_length + 1, sizeof(char));
    strcpy(str, "section .data\n");

    for (size_t i = 0; i < arraylist_size(section->data); i++) {
        AssemblyDataVariable *variable = arraylist_get(section->data, i);
        strcat(str, variable->label);
        strcat(str, ": ");
        strcat(str, _AssemblyDataSectionTypes_names[variable->type]);
        strcat(str, " ");
        strcat(str, variable->value);
        strcat(str, "\n");
    }

    return str;
}

void data_section_free(DataSection *section) {
    if (section == NULL) {
        return;
    }

    if (section->data != NULL) {
        for (size_t i = 0; i < section->data->size; i++) {
            AssemblyDataVariable *variable = arraylist_get(section->data, i);
            free(variable->label);
            free(variable->value);
            free(variable);
        }
        arraylist_free(section->data);
    }

    free(section);
}

// implementation of assembly/data.h

void assembly_data_variable_free(AssemblyDataVariable *variable) {
    free(variable->label);
    free(variable->value);
    free(variable);
}

AssemblyDataVariable *assembly_data_variable_new(char *label, AssemblyDataSectionTypes type, char *value) {
    AssemblyDataVariable *variable = calloc(1, sizeof(AssemblyDataVariable));
    variable->label = strdup(label);
    variable->type = type;
    variable->value = strdup(value);
    return variable;
}

