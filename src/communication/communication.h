#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stddef.h>
#include "../core/core.h"

/* small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
struct _RespBuf {
	char *buf;
	size_t cap;
	size_t len;
};

#define t_mth _web_request_method
typedef enum {
	GET,
	POST,
	DELETE,
} _web_request_method;

size_t _write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
void _run_curl(t_ctx ctx, char *path, char *data, cJSON **response_json, t_mth method);
#define _rcs(ctx, path, data, response, method) _run_curl_session(ctx, path, data, response, method)
void _run_curl_session(t_ctx ctx, char *path, char *data, cJSON **response, t_mth method);
int _gecko_run(t_ctx ctx, int force_kill);
int _wait_for_gecko_ready(t_ctx * ctx);
void _debug_response(cJSON * response_json);

#endif				// COMMUNICATION_H
