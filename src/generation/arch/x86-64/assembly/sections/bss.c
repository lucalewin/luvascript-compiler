#include <assembly/section.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>
#include <util/util.h>
#include <assembly/datatypes.h>

BssSection *bss_section_new() {
    BssSection *section = calloc(1, sizeof(BssSection));
    return section;
}

void bss_section_add_label(BssSection *section, const char *label) {

}

void bss_section_declare_space(BssSection *section, char *label, AssemblyBssSectionTypes type, size_t count) {

}

char *bss_section_to_string(BssSection *section) {
    return strdup("section .bss\n");
}

void bss_section_free(BssSection *section) {
    if (section == NULL) {
        return;
    }

    free(section);
}
