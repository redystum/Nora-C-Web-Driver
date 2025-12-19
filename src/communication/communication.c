#include <stdio.h>
#include "communication.h"

size_t _write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    struct _RespBuf *rb = (struct _RespBuf *) userdata;
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

void _run_curl(t_ctx ctx, char *path, char *data, char *response, t_mth method) {
    DEBUG("path='%s' data='%s'", path ? path : "(null)",
          data ? data : "(null)");
    CURL *curl = curl_easy_init();
    if (!curl) {
        DEBUG("curl_easy_init failed");
        return;
    }

    char url[1024] = {0};
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

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    if (method == POST) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (data != NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        } else {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
        }
    } else if (method == DELETE) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        if (data != NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        } else {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
        }
    } else { // Default to GET
        DEBUG("setting method to GET");
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }

    /* prepare safe response buffer wrapper */
    struct _RespBuf rb;
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

void _run_curl_session(t_ctx ctx, char *path, char *data, char *response, t_mth method) {
    char url[2048];
    char p[1024];
    if (path && path[0] != '/') {
        sprintf(p, "/%s", path);
        path = p;
    }

    sprintf(url, "/session/%s%s", ctx.session_id, path ? path : "");
    DEBUG("session_url='%s' data='%s'",
          url ? url : "(null)", data ? data : "(null)");
    _run_curl(ctx, url, data, response, method);
}

int _gecko_run(t_ctx ctx, int force_kill) {
    if (force_kill) {
        char kill_cmd[256] = {0};
        sprintf(kill_cmd, "fuser -k -n tcp %d > /dev/null 2>&1", ctx.port);
        int res = system(kill_cmd);
        DEBUG("killed existing geckodriver on port %d, res=%d",
              ctx.port, res);
        sleep(1);
    }

    char cmd[2048] = {0};
    char command[1024] = {0};
    sprintf(command, "--port %d --binary %s > /dev/null 2>&1 &", ctx.port, ctx.firefoxPath);
    DEBUG("starting geckodriver with command fragment: %s", command);

    sprintf(cmd, "%s %s", ctx.geckodriverPath, command);
    DEBUG("executing command: %s", cmd ? cmd : "(null)");
    int res = system(cmd);
    DEBUG("geckodriver start command returned %d", res);
    return res;
}

int _wait_for_gecko_ready(t_ctx *ctx) {
    char response[2048] = {0};
    int max_retries = 30;
    int retry = 0;

    while (retry < max_retries) {
        sleep(1);
        memset(response, 0, sizeof(response));
        _run_curl(*ctx, "/session",
                  "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
                  response, POST);

        char *p = strstr(response, "\"sessionId\"");
        if (p != NULL) {
            DEBUG("geckodriver is ready");
            if (!p) {
                DEBUG("sessionId not found in response");
                return -2;
            }
            p = strchr(p, ':') + 2;
            char *end = strchr(p, '"');
            char session_id[128] = {0};
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
