#include <util.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <logger.h>

// ----------------------------------------------------------

void substring(char *source, char *target, int offset) {
    char *destination = target;
    int index = 0;
    while (index < offset) {
        destination[index] = source[index];
        index++;
    }
    destination[index] = '\0';
}

char *stradd(char *s1, char *s2) {
    const size_t a = strlen(s1);
    const size_t b = strlen(s2);
    const size_t size_ab = a + b + 1;

    s1 = realloc(s1, size_ab);

    memcpy(s1 + a, s2, b + 1);

	s1[size_ab - 1] = '\0';

    return s1;
}

void straddall(char *src, ...) {
    va_list args;
    va_start(args, src);

    char *str;
    while ((str = va_arg(args, char *)) != NULL) {
        src = stradd(src, str);
    }

    va_end(args);
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

char *int_to_string(int i) {
	int length = snprintf(NULL, 0, "%d", i);
	char* str = malloc(length + 1);
	snprintf(str, length + 1, "%d", i);
	return str;
}