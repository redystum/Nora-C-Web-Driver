#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stddef.h>
#include <stdio.h>
#include "../core/core.h"

/* small buffer wrapper so write_callback can append safely */
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
/**
 * \brief Run a curl request
 * \param ctx web context
 * \param path request path
 * \param data request data
 * \param response_json output response JSON
 * \param method request method
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int run_curl(web_context *ctx, char *path, char *data, cJSON **response_json, web_request_method method);
/**
 * \brief Run a curl request for a session
 * \param ctx web context
 * \param path request path
 * \param data request data
 * \param response output response JSON
 * \param method request method
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
#define RCS(ctx, path, data, response, method) run_curl_session(ctx, path, data, response, method)
/**
 * \brief Run a curl request for a session
 * \param ctx web context
 * \param path request path
 * \param data request data
 * \param response output response JSON
 * \param method request method
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int run_curl_session(web_context *ctx, char *path, char *data, cJSON **response, web_request_method method);
/**
 * \brief run geckodriver
 * \param ctx web context
 * \param force_kill force kill existing geckodriver instances
 * \param ignore_gecko_checks ignore geckodriver executable checks
 * \param ignore_firefox_checks ignore firefox executable checks
 * \return \b 0 = success \n\b -5 = geckodriver startup error \n\b -6 = could not read geckodriver log
 */
int gecko_run(web_context *ctx, int force_kill, int ignore_gecko_checks, int ignore_firefox_checks);
/**
 * \brief Wait until geckodriver is ready
 * \param ctx web context
 * \return \b 0 = success \n\b -5 = timeout
 */
int wait_for_gecko_ready(web_context *ctx);

#endif				// COMMUNICATION_H
