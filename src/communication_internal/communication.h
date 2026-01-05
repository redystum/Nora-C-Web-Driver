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
/**
 * \brief Validate if executable exists and is executable
 * \param path path to executable
 * \param error_msg output error message buffer
 * \param error_len size of error message buffer
 * \return \b 0 = success \n\b -1 = empty path \n\b -2 = file not found \n\b -3 = not executable \n\b -4 = command failed \n\b -5 = startup error \n\b -6 = log file read error
 */
int gecko_run(web_context *ctx, int force_kill);
/**
 * \brief Wait until geckodriver is ready
 * \param ctx web context
 * \return \b 0 = success \n\b -5 = timeout
 */
int wait_for_gecko_ready(web_context *ctx);

#endif				// COMMUNICATION_H
