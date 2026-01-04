#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stddef.h>
#include <stdio.h>
#include "../core/core.h"

/* small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
typedef struct {
	char *buf;
	size_t cap;
	size_t len;
} web_resp_buf;

typedef enum {
	WEB_GET,
	WEB_POST,
	WEB_DELETE,
} web_request_method;

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
int run_curl(web_context *ctx, char *path, char *data, cJSON **response_json, web_request_method method);
#define RCS(ctx, path, data, response, method) run_curl_session(ctx, path, data, response, method)
int run_curl_session(web_context *ctx, char *path, char *data, cJSON **response, web_request_method method);
int gecko_run(web_context *ctx, int force_kill);
int wait_for_gecko_ready(web_context *ctx);

#endif				// COMMUNICATION_H
