#ifndef CORE_H
#define CORE_H

#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../utils/utils.h"

#define t_ctx web_context
typedef struct {
	int port;
	char *geckodriverPath;
	char *firefoxPath;
	char *session_id;
} web_context;

typedef struct {
	char *ready;
	char *message;
} web_status;

typedef struct {
	int implicit_wait_ms;
	int page_load_ms;
	int script_ms;
} web_timeouts;

web_context web_init(char *geckodriverPath, char *firefoxPath, int port,
		     int force_kill);
int web_close(web_context * ctx);
int web_usleep(int microseconds);
void reset_web_context(t_ctx * ctx);
web_status web_get_status(web_context ctx);
web_timeouts web_get_timeouts(web_context ctx);
int web_set_timeouts(web_context ctx, web_timeouts timeouts);

#endif				// CORE_H
