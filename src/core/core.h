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
    int port;
    char *geckodriverPath;
    char *firefoxPath;
    web_session session;
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
web_status web_get_status(web_context ctx);
web_timeouts web_get_timeouts(web_context ctx);
int web_set_timeouts(web_context ctx, web_timeouts timeouts);
void free_status(web_status status);
web_session web_create_session(web_context ctx);
void web_free_session(web_session session);

#endif				// CORE_H
