#ifndef LUVA_CONSOLE
#define LUVA_CONSOLE

/**
 * executes the command (cmd) with following arguments
 * IMPORTANT: the arguments should ALWAYS be followed by NULL
 */
int exec(const char *cmd, ...);

#endif