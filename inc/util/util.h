#ifndef LUVA_UTIL_H
#define LUVA_UTIL_H

// ----------------------------------------------------------------

#ifndef bool
	#define bool int
#endif

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

// ----------------------------------------------------------------

typedef signed int int32_t;
typedef signed long int64_t;

// ----------------------------------------------------------------

void substring(char *source, char *target, int offset); 

char *stradd(char *s1, char *s2);
char *straddall(char *src, ...);

char *strdup(const char *src);
char *allocate_string(const char *str);

int strendswith(const char *str, const char *suffix);

int str_last_index_of(char *str, char x);

bool is_number(char c);
bool is_whitespace(char c);

bool arr_contains(char **arr, int arr_length, char *key);

int32_t get_int(char *c);
char *int_to_string(int i);

#endif // LUVA_UTIL_H