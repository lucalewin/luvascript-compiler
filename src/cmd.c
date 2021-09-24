#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmd.h>
#include <stdarg.h>
#include <logger.h>

int exec(const char *cmd, ...) {
    int ret;
    va_list args;
    char *arg;
    char *buffer;

    buffer = malloc(strlen(cmd));
    strcpy(buffer, cmd);
    va_start(args, cmd);

    while ((arg = va_arg(args, char *)) != NULL) {
        buffer = realloc(buffer, sizeof(char *) * (strlen(buffer) + strlen(arg) + 1));
        strcat(buffer, " ");
        strcat(buffer, arg);
    }

    log_cmd("%s\n", buffer);
    ret = system(buffer);

    free(buffer);
    va_end(args);
    return ret;
}