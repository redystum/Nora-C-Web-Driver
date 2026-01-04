#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stddef.h>
#include <stdio.h>
#include "../core/core.h"

/* small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
struct _RespBuf {
	char *buf;
	size_t cap;
	size_t len;
};

typedef enum {
	GET,
	POST,
	DELETE,
} _web_request_method;

size_t _write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
int _run_curl(web_context *ctx, char *path, char *data, cJSON **response_json, _web_request_method method);
#define _rcs(ctx, path, data, response, method) _run_curl_session(ctx, path, data, response, method)
int _run_curl_session(web_context *ctx, char *path, char *data, cJSON **response, _web_request_method method);
int _gecko_run(web_context *ctx, int force_kill);
int _wait_for_gecko_ready(web_context *ctx);

#endif				// COMMUNICATION_H
