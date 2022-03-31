#ifndef __LUVA_STRING_H__
#define __LUVA_STRING_H__

char *append_string(char *dest, const char *src);

char *escape(const char *str);
char *unescape(char *src);

char *to_asm_string(char *src, char *dest);
char *to_asm_escaped_string(int arch, char *src);

char unescape_char(const char *c);

#endif // __LUVA_STRING_H__