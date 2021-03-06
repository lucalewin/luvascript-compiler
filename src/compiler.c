#include "include/compiler.h"

char *assembler = "nasm -f elf64";
char *linker = "ld -g";

void compile(char *file_base_name) {
    /*
     *
     * compile .asm file using nasm and elf64 format
     * 
     */
    char *assemble = malloc(sizeof(assembler) + 2 * sizeof(file_base_name) + 11);
    sprintf(assemble, "%s %s.asm -o %s.o", assembler, file_base_name, file_base_name);
    // printf("assemble: %s\n", assemble);
    system(assemble);
    free(assemble);

    /*
     *
     * link .o file and create binary file
     * 
     */
    char *link = malloc(sizeof(linker) + 2 * sizeof(file_base_name) + 7);
    sprintf(link, "%s %s.o -o %s", linker, file_base_name, file_base_name);
    // printf("link: %s\n", link);
    system(link);
    free(link);

    // remove .asm and .o file(s)
    char *rm = "rm -rf";
    char *remove = malloc(sizeof(rm) + sizeof(file_base_name) + 9);
    sprintf(remove, "%s %s.o %s.asm", rm, file_base_name, file_base_name);
    // system(remove);
    free(remove);

}