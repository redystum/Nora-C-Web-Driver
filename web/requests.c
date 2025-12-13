#include "web.h"

static size_t _write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
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

void _run_curl(t_ctx ctx, char *path, char *data, char *response) {
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

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_callback);
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

void _run_curl_session(t_ctx ctx, char *path, char *data, char *response) {
	char *url = NULL;
	char *p = NULL;
	if (path && path[0] != '/') {
		ut_str_cat(&p, "/", path, NULL);
		path = p;
	}

	ut_str_cat(&url, "/session/", ctx.session_id, path, NULL);
	INFO("session_url='%s' data='%s'",
	     url ? url : "(null)", data ? data : "(null)");
	_run_curl(ctx, url, data, response);
	if (url)
		free(url);
}