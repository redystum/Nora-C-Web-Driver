#include "web.h"
#include "utils.h"
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
struct RespBuf {
	char *buf;
	size_t cap;
	size_t len;
};

void reset_web_context(t_ctx* ctx) {
	if (ctx->session_id) {
		free(ctx->session_id);
		ctx->session_id = NULL;
	}
	if (ctx->current_url) {
		free(ctx->current_url);
		ctx->current_url = NULL;
	}
}

int gecko(t_ctx ctx, int force_kill) {
	if (force_kill) {
		char kill_cmd[256];
		sprintf(kill_cmd, "fuser -k -n tcp %d > /dev/null 2>&1", ctx.port);
		int res = system(kill_cmd);
		DEBUG("killed existing geckodriver on port %d, res=%d",
		      ctx.port, res);
		sleep(1);
	}
	char *cmd = NULL;

	char command[1024];
	sprintf(command, "--port %d --binary %s > /dev/null 2>&1 &", ctx.port, ctx.firefoxPath);
	DEBUG("starting geckodriver with command fragment: %s", command);

	ut_str_cat(&cmd, ctx.geckodriverPath, " ", command, NULL);
	DEBUG("executing command: %s", cmd ? cmd : "(null)");
	int res = system(cmd);
	DEBUG("geckodriver start command returned %d", res);
	if (cmd) free(cmd);
	return res;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb,
			     void *userdata) {
	size_t total = size * nmemb;
	struct RespBuf *rb = (struct RespBuf *)userdata;
	if (!rb || !rb->buf) {
		return total;
	}
	/* compute how many bytes we can copy without overflowing (leave room for NUL)
	 */
	size_t avail = (rb->cap > rb->len) ? (rb->cap - rb->len - 1) : 0;
	size_t to_copy = total < avail ? total : avail;
	if (to_copy > 0) {
		memcpy(rb->buf + rb->len, ptr, to_copy);
		rb->len += to_copy;
		rb->buf[rb->len] = '\0';
	}
	/* if overflow would have occurred, we simply truncate silently */
	DEBUG("received %zu bytes, appended %zu bytes, len=%zu",
	      total, to_copy, rb->len);
	return total;
}

void run_curl(t_ctx ctx, char *path, char *data, char *response) {
	DEBUG("path='%s' data='%s'", path ? path : "(null)",
	      data ? data : "(null)");
	CURL *curl = curl_easy_init();
	if (!curl) {
		DEBUG("curl_easy_init failed");
		return;
	}

	char url[1024] = { 0 };
	if (path && path[0] == '/') {
		DEBUG("path starts with /");
		sprintf(url, "http://127.0.0.1:%d%s", ctx.port, path);
	} else if (path) {
		DEBUG("path does not start with /");
		sprintf(url, "http://127.0.0.1:%d/%s", ctx.port, path);
	}
	DEBUG("url='%s'", url);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	if (data != NULL) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
	}

	/* prepare safe response buffer wrapper */
	struct RespBuf rb;
	rb.buf = response;
	rb.cap = RESPONSE_CAP;
	/* ensure existing response buffer is NUL-terminated and track current length
	 */
	if (response) {
		response[rb.cap - 1] = '\0';
		rb.len = strlen(response);
	} else {
		rb.len = 0;
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rb);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		DEBUG("curl_easy_perform failed: %s", curl_easy_strerror(res));
	} else {
		DEBUG("request OK, response (truncated): %.200s",
		      response ? response : "(null)");
	}
	curl_easy_cleanup(curl);
}

#define _rcs(ctx, path, data, response) run_curl_session(ctx, path, data, response)
void run_curl_session(t_ctx ctx, char *path, char *data, char *response) {
	char *url = NULL;
	char *p = NULL;
	if (path && path[0] != '/') {
		ut_str_cat(&p, "/", path, NULL);
		path = p;
	}

	ut_str_cat(&url, "/session/", ctx.session_id, path, NULL);
	INFO("session_url='%s' data='%s'",
	     url ? url : "(null)", data ? data : "(null)");
	run_curl(ctx, url, data, response);
	if (url)
		free(url);
}

int wait_for_gecko_ready(t_ctx* ctx) {
	char response[2048] = { 0 };
	int max_retries = 30;
	int retry = 0;

	while (retry < max_retries) {
		sleep(1);
		memset(response, 0, sizeof(response));
		run_curl(*ctx, "/session",
			 "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
			 response);

		char *p = strstr(response, "\"sessionId\"");
		if (p != NULL) {
			DEBUG("geckodriver is ready");
			if (!p) {
				DEBUG("sessionId not found in response");
				return -2;
			}
			p = strchr(p, ':') + 2;
			char *end = strchr(p, '"');
			char session_id[128];
			strncpy(session_id, p, end - p);
			session_id[end - p] = 0;
			ctx->session_id = strdup(session_id);
			return 0;
		}
		retry++;
		DEBUG("geckodriver not ready yet, retry %d/%d",
		      retry, max_retries);

		sleep(1);
	}

	DEBUG("geckodriver failed to start after %d attempts", max_retries);
	return -1;
}

/**
 * \brief Initialize web context
 * \param geckodriverPath NULL = "geckodriver"
 * \param firefoxPath NULL = "firefox"
 * \param port 0 = 9515
 * \param force_kill 1 = kill existing geckodriver on port
 * \return
 */
web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill) {
	t_ctx ctx = {0};
	if (geckodriverPath != NULL) {
		ctx.geckodriverPath = geckodriverPath;
	} else {
		ctx.geckodriverPath = "geckodriver";
	}
	if (firefoxPath != NULL) {
		ctx.firefoxPath = firefoxPath;
	} else {
		ctx.firefoxPath = "firefox";
	}
	if (port != 0) {
		ctx.port = port;
	} else {
		ctx.port = 9515;
	}

	if (gecko(ctx, force_kill) < 0) {
		DEBUG("Failed to start geckodriver");
		reset_web_context(&ctx);
		return ctx;
	}

	if (wait_for_gecko_ready(&ctx) < 0) {
		DEBUG("Geckodriver failed to start");
		reset_web_context(&ctx);
		return ctx;
	}

	DEBUG("extracted session_id='%s'", ctx.session_id);

	return ctx;
}

int web_open(web_context ctx, char *link) {
	char response[2048] = { 0 };
	char *data = NULL;
	ut_str_cat(&data, "{\"url\": \"", link, "\"}", NULL);
	DEBUG("opening link='%s' data='%s'", link ? link : "(null)",
	      data ? data : "(null)");
	_rcs(ctx, "/url", data, response);
	DEBUG("response (truncated): %.200s", response);
	free(data);
	return 0;
}

int web_close(web_context *ctx) {
	DEBUG("closing session '%s'", ctx->session_id);
	CURL *curl = curl_easy_init();

	char endpoint[256];
	sprintf(endpoint, "http://127.0.0.1:%d/session/%s", ctx->port, ctx->session_id);
	DEBUG("endpoint='%s'", endpoint);

	curl_easy_setopt(curl, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	reset_web_context(ctx);

	return 0;
}
