#include <util/file.h>

#include <stdio.h>
#include <stdlib.h>

#include <logging/logger.h>

char *read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        log_error("could not open file '%s'\n", path);
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read file
    char *buffer = (char*) malloc(sizeof(char) * (size + 1));
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}

void write_file(const char* path, char *text) {
    FILE *file = fopen(path, "w");
    if (!file) {
        log_error("write_file(): could not open file '%s'\n", path);
        return;
    }
    fputs(text, file);
    fclose(file);
}