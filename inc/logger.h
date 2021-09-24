#ifndef LUVA_DEBUGGER
#define LUVA_DEBUGGER

void print_info(const char *message, ...);
void print_warning(const char *message, ...);
void print_error(const char *message, ...);
void print_debug(const char *message, ...);
void print_cmd(const char *command, ...);

#endif