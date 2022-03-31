#include <util/string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *unescape(char *src) {
    char *dst = src;
    char *src_end = src + strlen(src);
    char *start = src;
    while (src < src_end) {
        if (*src == '\\') {
            src++;
            switch (*src) {
                case 'n':
                    *dst = '\n';
                    printf("newline\n");
                    break;
                case 'r':
                    *dst = '\r';
                    break;
                case 't':
                    *dst = '\t';
                    break;
                case 'b':
                    *dst = '\b';
                    break;
                case '0':
                    *dst = '\0';
                    break;
                case '\\':
                    *dst = '\\';
                    break;
                default:
                    *dst = *src;
                    break;
            }
        } else {
            *dst = *src;
        }
        src++;
        dst++;
    }
    *dst = '\0';
    return start;
}

char *escape(const char *str) {
    return NULL;
}

char *to_asm_string(char *src, char *dest) {
    // char *src_end = src + strlen(src);
    
    dest = calloc(strlen(dest), 1);

    // while (src < src_end) {
    //     if (*src == '\\') {
    //         src++;
    //         switch (*src) {
    //             case 'n':
    //                 append_string(dest, "\",");
    //                 break;
    //             case 'r':
    //                 *dst = '\r';
    //                 break;
    //             case 't':
    //                 *dst = '\t';
    //                 break;
    //             case 'b':
    //                 *dst = '\b';
    //                 break;
    //             case '0':
    //                 *dst = '\0';
    //                 break;
    //             case '\\':
    //                 *dst = '\\';
    //                 break;
    //             default:
    //                 *dst = *src;
    //                 break;
    //         }
    //     } else {
    //         append_string(dst, *src);
    //     }
    //     src++;
    // }

    return dest;
}

char *append_string(char *dest, const char *src) {
    dest = realloc(dest, strlen(dest) + strlen(src) + 1);
    strcat(dest, src);
    return dest;
}

char *to_asm_escaped_string(int arch, char *src) {
    int max_length = arch == 64 ? 8 : 4;

    int src_length = strlen(src);
    int dest_length = src_length / max_length * 3 + src_length + 2;

    int src_index = 0;
    int dest_index = 1;

    char *dest = calloc(dest_length + 1, sizeof(char));

    dest[0] = '`';

    while (src_index < src_length) {
        dest[dest_index++] = src[src_index++];

        if (src_index % max_length == 0) {
            dest[dest_index++] = '`';
            dest[dest_index++] = ',';
            dest[dest_index++] = '`';
        }

    }

    dest[dest_length - 2] = '`';
    dest[dest_length - 1] = '\0';

    return dest;
}


char unescape_char(const char *c) {
    if (*c != '\\') return *c;
    switch (*++c) {
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'b':
            return '\b';
        case '0':
            return '\0';
        case '\\':
            return '\\';
        default:
            return *c;
    }
}