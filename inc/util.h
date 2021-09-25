#ifndef LUVA_UTIL_H
#define LUVA_UTIL_H

// ----------------------------------------------------------------

#define bool int

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

// ----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------

typedef signed int int32_t;
typedef signed long int64_t;

// ----------------------------------------------------------------

char *read_file(const char* path);
void write_file(const char* path, char *text);

//char *_substring(char *source, char *target, int from, int to);
void substring(char *source, char *target, int offset); 

void stradd(char *src, char *str);

int str_last_index_of(char *str, char x);

bool is_number(char c);
bool is_whitespace(char c);

bool arr_contains(char **arr, int arr_length, char *key);

int32_t get_int(char *c);

#endif // LUVA_UTIL_H