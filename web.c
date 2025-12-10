#include "web.h"
#include "utils.h"
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char *_gecko = "geckodriver";
char *_firefox = "firefox";
char _session_id[200];

/* Add a small buffer wrapper so write_callback can append safely */
#define RESPONSE_CAP 2048
struct RespBuf {
	char *buf;
	size_t cap;
	size_t len;
};

int gecko(char *command) {
	char *cmd = NULL;
	ut_str_cat(&cmd, _gecko, " ", command, NULL);
	DEBUG("gecko: executing command: %s", cmd ? cmd : "(null)");
	return system(cmd);
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
	DEBUG("write_callback: received %zu bytes, appended %zu bytes, len=%zu",
	      total, to_copy, rb->len);
	return total;
}

void run_curl(char *path, char *data, char *response) {
	DEBUG("run_curl: path='%s' data='%s'", path ? path : "(null)",
	      data ? data : "(null)");
	CURL *curl = curl_easy_init();
	if (!curl) {
		DEBUG("run_curl: curl_easy_init failed");
		return;
	}

	char *url = NULL;
	if (path && path[0] == '/') {
		ut_str_cat(&url, "http://127.0.0.1:9515", path, NULL);
	} else if (path) {
		ut_str_cat(&url, "http://127.0.0.1:9515/", path, NULL);
	}

	DEBUG("run_curl: url='%s'", url ? url : "(null)");

	curl_easy_setopt(curl, CURLOPT_URL, url);
	// Set content type to application/json
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
		DEBUG("run_curl: curl_easy_perform failed: %s",
		      curl_easy_strerror(res));
	} else {
		DEBUG("run_curl: request OK, response (truncated): %.200s",
		      response ? response : "(null)");
	}
	curl_easy_cleanup(curl);
	if (url)
		free(url);
}

#define _rcs(path, data, response) run_curl_session(path, data, response)
void run_curl_session(char *path, char *data, char *response) {
	char *url = NULL;
	char *p = NULL;
	if (path && path[0] != '/') {
		ut_str_cat(&p, "/", path, NULL);
		path = p;
	}

	ut_str_cat(&url, "/session/", _session_id, path, NULL);
	INFO("run_curl_session: session_url='%s' data='%s'",
	     url ? url : "(null)", data ? data : "(null)");
	run_curl(url, data, response);
	if (url)
		free(url);
}

int web_init(char *geckodriverPath, char *firefoxPath) {
	if (geckodriverPath != NULL) {
		_gecko = geckodriverPath;
	}
	if (firefoxPath != NULL) {
		_firefox = firefoxPath;
	}

	char *command = NULL;
	ut_str_cat(&command, "--port 9515 --binary ", _firefox,
		   " > /dev/null 2>&1 &", NULL);
	DEBUG("web_init: starting geckodriver with command fragment: %s",
	      command ? command : "(null)");
	gecko(command);

	/* wait a bit for geckodriver to start */
	sleep(10);

	char response[2048] = { 0 };

	run_curl("/session",
		 "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
		 response);

	DEBUG("web_init: session creation response (truncated): %.500s",
	      response);

	char *p = strstr(response, "\"sessionId\"");
	if (!p) {
		DEBUG("web_init: sessionId not found in response");
		return -2;
	}
	p = strchr(p, ':') + 2;
	char *end = strchr(p, '"');
	strncpy(_session_id, p, end - p);
	_session_id[end - p] = 0;

	DEBUG("web_init: extracted session_id='%s'", _session_id);

	return 0;
}

int web_open(char *link) {
	char response[2048] = { 0 };
	char *data = NULL;
	ut_str_cat(&data, "{\"url\": \"", link, "\"}", NULL);
	DEBUG("web_open: opening link='%s' data='%s'", link ? link : "(null)",
	      data ? data : "(null)");
	_rcs("/url", data, response);
	DEBUG("web_open: response (truncated): %.200s", response);
	free(data);
	return 0;
}

int web_close(void) {
	DEBUG("web_close: closing session '%s'", _session_id);
	CURL *curl = curl_easy_init();

	char endpoint[256];
	sprintf(endpoint, "http://127.0.0.1:9515/session/%s", _session_id);
	DEBUG("web_close: endpoint='%s'", endpoint);

	curl_easy_setopt(curl, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return 0;
}
