#include <util/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <util/logging/logger.h>

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
	int length = snprintf(NULL, 0, "%s%s", s1, s2);
	char* str = calloc(length + 1, sizeof(char));
	snprintf(str, length + 1, "%s%s", s1, s2);
	return str;
}

char *straddall(char *src, ...) {
    va_list args;
    va_start(args, src);

    char *str;
    while ((str = va_arg(args, char *)) != NULL) {
        src = stradd(src, str);
    }

    va_end(args);

	return src;
}

char *strdup(const char *src) {
    if (src == NULL) return NULL;
	char *dst = calloc(strlen (src) + 1, sizeof (char));  // Space for length plus nul
	if (dst == NULL) return NULL;          // No memory
	strcpy(dst, src);                      // Copy the characters
	return dst;                            // Return the new string
}

char *allocate_string(const char *str) {
	char *new_str = calloc(strlen(str) + 1, sizeof(char));
	strcpy(new_str, str);
	return new_str;
}

/*
 * Edited from https://stackoverflow.com/a/9693452/13990026
 */
char** copy_all(char** argv, int length) {
    if (argv == NULL) return NULL;
    if (length == 0) return NULL;
    
    char** ppDest = calloc(length, sizeof(char**));
    if (!ppDest) {
        return NULL;
    }

    int i = 0;
    for (; i < length; i++) {
        char* pCurrent = argv[i];
        size_t currentLength = strlen(pCurrent);
        ppDest[i] = calloc(currentLength + 1, sizeof(char));
        if (!ppDest[i]) {
            goto Error;
        }
        strcpy(ppDest[i], argv[i]);
    }

    return ppDest;

Error:
    while (i > 0) {
        free(ppDest[i - 1]);
        i--;
    }
    free(ppDest);
    return NULL;
}

// ----------------------------------------------------------

int strendswith(const char *str, const char *suffix)
{
	if (str == NULL || suffix == NULL)
	{
		return FALSE;
	}

	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);

	if (suffix_len > str_len)
	{
		return FALSE;
	}

	return strcmp(str + str_len - suffix_len, suffix) == 0;
}

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
	char* str = calloc(length + 1, sizeof(char));
	snprintf(str, length + 1, "%d", i);
	return str;
}

int min(int x, int y) {
    return x < y ? x : y;
}

int max(int x, int y) {
    return x > y ? x : y;
}
