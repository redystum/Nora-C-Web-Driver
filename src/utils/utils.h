#ifndef UTILS_H
#define UTILS_H

#include <cjson/cJSON.h>

/**
 * \brief Prints an error message and aborts the program (PANIC)
 * \param code code the error code
 * \param fmt format string
 * \param ... ...
 */
#define ERROR(code, ...) error(__FILE__, __LINE__, code, __VA_ARGS__)

/**
 * \brief Prints a warning message
 * \param fmt format string
 * \param ... ...
 */
#define WARNING(...) warning(__VA_ARGS__)

/**
 * \brief Prints an info message
 * \param fmt format string
 * \param ... ...
 */
#define INFO(...) info(__VA_ARGS__)

#ifdef DEBUG_ENABLED
/**
 * \brief Prints a debug message if DEBUG_ENABLED is defined
 * \param fmt format string
 * \param ... ...
 */
#define DEBUG(...) debug(__FILE__, __LINE__, __func__, __VA_ARGS__)
/**
 * \brief Prints a cJSON object for debugging
 * \param json  the cJSON object to debug
 */
#define DEBUG_JSON(json) debug_json(json, __FILE__, __LINE__, __func__)
#else
/**
 * \brief Prints a debug message if DEBUG_ENABLED is defined
 * \param fmt format string
 * \param ... ...
 */
#define DEBUG(...)
/**
 * \brief Prints a cJSON object for debugging
 * \param json  the cJSON object to debug
 */
#define DEBUG_JSON(json)
#endif

/**
 * \brief Initializes the log file
 * \param path  the path of the log file
 */
void ut_file_log_init(const char *path);

/**
 * \brief Gets the path of the log file
 * \return  the path of the log file
 */
char *ut_get_file_log_path();

/**
 * \brief Frees resources used by the log file
 */
void ut_file_log_free();

/*
 * Logs a message to the log file
 *
 * @param text the message to log
 */
#define EOL '\n'

void error(char *file, int line, int code, char *fmt, ...);
void warning(char *format, ...);
void info(char *format, ...);
void debug(char *file, int line, const char *func, char *format, ...);
void debug_json(cJSON *json, char *file, int line, const char *func);

#endif				// UTILS_H
