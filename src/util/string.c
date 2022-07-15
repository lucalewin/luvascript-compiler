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