#include <stdlib.h>
#include <string.h>
#include <compiler.h>
#include <cmd.h>

char *assembler = "nasm";
char *file_format = "-f elf64";
char *linker = "ld";

void compile_asm(char *src_file, char *out_file) {
    // get temp object file name
    char *obj_file = malloc(sizeof(obj_file) * (strlen(src_file) + 2));
    strcpy(obj_file, src_file);
    strcat(obj_file, ".o");

    // compile .asm file using nasm and elf64 format
    exec(assembler, file_format, src_file, "-o", obj_file, NULL);
    
    // link .o file and create binary file
    exec(linker, "-g", obj_file, out_file);
}