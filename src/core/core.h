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
    char *url;
    int code;
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

web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill);
int web_close(web_context *ctx);
int web_usleep(int microseconds);
void web_reset_context(web_context *ctx);
web_status web_get_status(web_context *ctx);
web_timeouts web_get_timeouts(web_context *ctx);
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


#endif				// CORE_H
