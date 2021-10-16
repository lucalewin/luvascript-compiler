#ifndef LUVA_CONSOLE_H
#define LUVA_CONSOLE_H

/**
 * @brief executes the command (cmd) with following arguments
 * IMPORTANT: the arguments should ALWAYS be followed by NULL 
 * 
 * @param cmd the command name
 * @param ... command arguments -> should *always* be followed by NULL
 * @return int the exit code of the executed command
 */
int exec(const char *cmd, ...);

#endif