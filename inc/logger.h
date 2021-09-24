#ifndef LUVA_DEBUGGER
#define LUVA_DEBUGGER

void log_info(const char *message, ...);
void log_warning(const char *message, ...);
void log_error(const char *message, ...);
void log_debug(const char *message, ...);
void log_cmd(const char *command, ...);

#endif