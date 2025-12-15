#ifndef WEB_H
#define WEB_H

#include "../utils.h"
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define t_ctx web_context
typedef struct {
	int port;
	char *geckodriverPath;
	char *firefoxPath;
	char *session_id;
} web_context;

web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill);
int web_open(web_context ctx, char *link);
int web_change_url(web_context ctx, char *link);
char* web_get_current_url(web_context ctx);
int wait_to_page_load(web_context ctx, int max_wait_seconds);
int web_close(web_context *ctx);
int web_usleep(int microseconds);

// privates

/* small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
struct RespBuf {
	char *buf;
	size_t cap;
	size_t len;
};

size_t _write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
void _run_curl(t_ctx ctx, char *path, char *data, char *response);
#define _rcs(ctx, path, data, response) _run_curl_session(ctx, path, data, response)
void _run_curl_session(t_ctx ctx, char *path, char *data, char *response);
void reset_web_context(t_ctx* ctx);
int _gecko_run(t_ctx ctx, int force_kill);
int _wait_for_gecko_ready(t_ctx* ctx);
#endif				// WEB_H
