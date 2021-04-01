//#include "include/util.h"

#include <util.h>

// ----------------------------------------------------------

char *read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read file
    char *buffer = (char*) malloc(sizeof(char) * (size + 1));
    size_t t = fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}

// ----------------------------------------------------------

void write_file(const char* path, char *text) {
    FILE *file = fopen(path, "w");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return;
    }
    fputs(text, file);
    fclose(file);
}

// ----------------------------------------------------------

void substring(char *source, char *target, int offset) {
    char *destination = target;
    int index = 0;
    while (index < offset) {
        destination[index] = source[index];
        index++;
    }
    destination[index] = '\0';
    // return destination;
}

// ----------------------------------------------------------

int str_last_index_of(char *str, char x) { 
    // Traverse from right 
    int str_len = strlen(str);
    for (int i = str_len - 1; i >= 0; i--) 
        if (str[i] == x) 
            return i; 
  
    return -1; 
}

// ----------------------------------------------------------

bool is_number(char c) {
    if (c && c >= '0' && c <= '9') {
        return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------

bool is_whitespace(char c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------

bool arr_contains(char **arr, int arr_length, char *key) {
    for (int i = 0; i < arr_length; i++) {
        if (strcmp(arr[i], key) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// ----------------------------------------------------------

int32_t get_int(char *c) {
    int num = 0;
    while (is_number(*c)) {
        num = num * 10 + (*c++ - '0');
    }
    return num;
}