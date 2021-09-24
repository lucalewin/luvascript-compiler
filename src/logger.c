#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <stdarg.h>
#include <string.h>

void priv_print(const char *prefix, const char *message, va_list args);

void priv_print(const char *prefix, const char *message, va_list args) {
    const char *format = "%s %s";
    char *fmt = malloc(strlen(prefix) + strlen(message));
    sprintf(fmt, format, prefix, message);
    vprintf(fmt, args);
}

void log_info(const char *message, ...) {
    /* Declare a va_list type variable */
    va_list args;
    /* Initialise the va_list variable with the ... after message */
    va_start(args, message);
    priv_print("[INFO]:", message, args);
    /* Clean up the va_list */
    va_end(args);
}

void log_warning(const char *message, ...) {
    // printf("[WARNING]: %s", message);
    /* Declare a va_list type variable */
    va_list args;

    /* Initialise the va_list variable with the ... after message */
    va_start(args, message);

    /* Forward the '...' to vprintf */
    priv_print("[WARNING]:", message, args);

    /* Clean up the va_list */
    va_end(args);
}

void log_error(const char *message, ...) {
    // printf("[ERROR]: %s", message);
    /* Declare a va_list type variable */
    va_list args;

    /* Initialise the va_list variable with the ... after message */
    va_start(args, message);

    /* Forward the '...' to vprintf */
    priv_print("[ERROR]:", message, args);

    /* Clean up the va_list */
    va_end(args);
}

void log_debug(const char *message, ...) {
    // printf("[DEBUG]: %s", message);
    /* Declare a va_list type variable */
    va_list args;

    /* Initialise the va_list variable with the ... after message */
    va_start(args, message);

    /* Forward the '...' to vprintf */
    priv_print("[DEBUG]:", message, args);

    /* Clean up the va_list */
    va_end(args);
}

void log_cmd(const char *message, ...) {
    // printf("[CMD]: %s", command);
    /* Declare a va_list type variable */
    va_list args;

    /* Initialise the va_list variable with the ... after message */
    va_start(args, message);

    /* Forward the '...' to vprintf */
    priv_print("[CMD]:", message, args);

    /* Clean up the va_list */
    va_end(args);
}