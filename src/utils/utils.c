#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_PURPLE "\x1b[35m"
#define COLOR_RESET "\x1b[0m"

int can_log = 0;

void _file_log(char *tag, char *text, va_list args);

void error(char *file, int line, int code, char *fmt, ...) {
    va_list args, args_copy;

    va_start(args, fmt);
    va_copy(args_copy, args);
    fprintf(stderr, COLOR_RED "[E %s:%d]\t ", file, line);
    vfprintf(stderr, fmt, args);
    _file_log("[E]\t ", fmt, args_copy);
    va_end(args_copy);
    va_end(args);
    fprintf(stderr, "\nAborting\n" COLOR_RESET);
    exit(code);
}

void warning(char *format, ...) {
    va_list args, args_copy;

    va_start(args, format);
    va_copy(args_copy, args);
    fprintf(stderr, COLOR_YELLOW "[W]\t ");
    vfprintf(stderr, format, args);
    _file_log("[W]\t ", format, args_copy);
    va_end(args_copy);
    va_end(args);
    fprintf(stderr, "\n" COLOR_RESET);
    fflush(stderr);
}

void info(char *format, ...) {
    va_list args, args_copy;

    va_start(args, format);
    va_copy(args_copy, args);
    fprintf(stdout, COLOR_BLUE "[I]\t ");
    vfprintf(stdout, format, args);
    _file_log("[I]\t ", format, args_copy);
    va_end(args_copy);
    va_end(args);
    fprintf(stdout, "\n" COLOR_RESET);
    fflush(stdout);
}

void debug(char *file, int line, const char *func, char *format, ...) {
    va_list args;

    va_start(args, format);
    fprintf(stdout, COLOR_PURPLE "[%s:%d] %s: ", file, line, func);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n" COLOR_RESET);
    fflush(stdout);
}

char *log_file_path = NULL;

void ut_file_log_init(const char *path) {
    if (path == NULL) {
        can_log = 0;
        return;
    }

    log_file_path = strdup(path);
    if (log_file_path == NULL) {
        ERROR(1, "Error allocating memory for log file path");
    }

    FILE *file = fopen(log_file_path, "w");
    if (file == NULL) {
        ERROR(1, "Error opening log file");
    }
    fclose(file);

    info("Log file initialized at %s", log_file_path);
    can_log = 1;
}

void _file_log(char *tag, char *text, va_list args) {
    if (can_log == 0) {
        return;
    }

    if (log_file_path == NULL) {
        ERROR(1, "Log file path not initialized");
    }
    FILE *file = fopen(log_file_path, "a");
    if (file == NULL) {
        ERROR(1, "Error opening log file");
    }

    fprintf(file, "%s", tag);
    vfprintf(file, text, args);
    fprintf(file, "\n");

    fclose(file);
}

char *ut_get_file_log_path() {
    return log_file_path;
}

void ut_file_log_free() {
    if (log_file_path != NULL) {
        free(log_file_path);
    }
}
