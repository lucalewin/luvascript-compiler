#ifndef LUVA_DEBUGGER
#define LUVA_DEBUGGER

#define RED    "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE   "\x1B[34m"
#define MAGMA  "\x1B[35m"
#define CYAN   "\x1B[36m"
#define WHITE  "\x1B[37m"
#define RESET  "\x1B[0m"

#define IRED   "\x1B[91m"

void log_info(const char *message, ...);
void log_warning(const char *message, ...);
void log_error(const char *message, ...);
void log_debug(const char *message, ...);
void log_cmd(const char *command, ...);

#endif