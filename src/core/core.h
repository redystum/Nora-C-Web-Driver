#ifndef CORE_H
#define CORE_H

#include <curl/curl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <cjson/cJSON.h>

#include "../utils/utils.h"

typedef struct {
    char *id;
    cJSON *capabilities;
} web_session;

typedef struct {
    int code;
    char *path;
    char *error;
    char *message;
} web_error;

typedef struct {
    int port;
    char *geckodriverPath;
    char *firefoxPath;
    web_session session;

    web_error last_error;
} web_context;

typedef struct {
    bool ready;
    char *message;
} web_status;

typedef struct {
    int implicit_wait_ms;
    int page_load_ms;
    int script_ms;
} web_timeouts;

/**
 * \brief Initialize web context
 * \param ctx web context to initialize
 * \param geckodriverPath path to geckodriver executable
 * \param firefoxPath path to firefox executable
 * \param port port to run geckodriver on
 * \param force_kill whether to force kill existing geckodriver on the port
 * \return \b 0 = success \n\b -1 = empty path \n\b -2 = file not found \n\b -3 = not executable \n\b -4 = command failed \n\b -5 = startup error \n\b -6 = log file read error
 */
int web_init(web_context *ctx, char *geckodriverPath, char *firefoxPath, int port, int force_kill);
/**
 * \brief Close web context
 * \param ctx web context to close
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_close(web_context *ctx);
/**
 * \brief Sleep for a given number of microseconds
 * \param microseconds number of microseconds to sleep
 * \return 0 on success, -1 on failure
 */
int web_usleep(int microseconds);
/**
 * \brief Sleep for a given number of seconds
 * \param seconds number of seconds to sleep
 * \return 0 on success, -1 on failure
 */
int web_sleep(int seconds);
/**
 * \brief Reset web context
 * \param ctx web context to reset
 */
void web_reset_context(web_context *ctx);
/**
 * \brief Get web status
 * \param ctx web context
 * \param status pointer to web status to fill
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_status(web_context *ctx, web_status *status);
/**
 * \brief Get web timeouts
 * \param ctx web context
 * \param timeouts pointer to web timeouts to fill
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_timeouts(web_context *ctx, web_timeouts *timeouts);
/**
 * \brief Set web timeouts
 * \param ctx web context
 * \param timeouts web timeouts to set
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_set_timeouts(web_context *ctx, web_timeouts timeouts);
/**
 * \brief Create a new web session
 * \param ctx web context
 * \param session pointer to web session to fill
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_create_session(web_context *ctx, web_session *session);
/**
 * \brief Free web session
 * \param session web session
 */
void web_free_session(web_session session);
/**
 * \brief Free web status
 * \param status web status
 */
void free_status(web_status status);
/**
 * \brief Get last error from web context
 * \param ctx web context
 * \return web error
 */
web_error web_get_last_error(web_context *ctx);
/**
 * \brief Reset last error in web context
 * \param ctx web context
 */
void web_reset_last_error(web_context *ctx);
/**
 * \brief
 * \param ctx web context
 * \param error web error
 */
void web_set_last_error(web_context *ctx, web_error error);

#endif				// CORE_H
