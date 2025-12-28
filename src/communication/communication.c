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

int _run_curl(web_context ctx, char *path, char *data, cJSON **response_json, t_mth method) {
    DEBUG("path='%s' data='%s'", path ? path : "(null)",
          data ? data : "(null)");
    CURL *curl = curl_easy_init();
    if (!curl) {
        DEBUG("curl_easy_init failed");
        return -2;
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
    } else {
        // Default to GET
        DEBUG("setting method to GET");
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }

    /* prepare safe response buffer wrapper */
    char raw_buf[RESPONSE_CAP] = {0};
    struct _RespBuf rb;
    rb.buf = raw_buf;
    rb.cap = RESPONSE_CAP;
    rb.len = 0;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rb);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        DEBUG("curl_easy_perform failed: %s", curl_easy_strerror(res));
        return -1;
    } else {
        DEBUG("request OK, response (truncated): %.200s", raw_buf);
        if (response_json) {
            *response_json = cJSON_Parse(raw_buf);
        }
    }
    curl_easy_cleanup(curl);
    return 0;
}

void _run_curl_session(web_context ctx, char *path, char *data, cJSON **response, t_mth method) {
    char url[2048];
    char p[1024];
    if (path && path[0] != '/') {
        sprintf(p, "/%s", path);
        path = p;
    }

    sprintf(url, "/session/%s%s", ctx.session.id, path ? path : "");
    DEBUG("session_url='%s' data='%s'",
          url ? url : "(null)", data ? data : "(null)");
    _run_curl(ctx, url, data, response, method);
}

int _gecko_run(web_context ctx, int force_kill) {
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

int _wait_for_gecko_ready(web_context *ctx) {
    cJSON *response = NULL;
    int max_retries = 30;
    int retry = 0;

    while (retry < max_retries) {
        web_session session = web_create_session(*ctx);
        DEBUG("session.id='%s'", session.id ? session.id : "(null)");
        WARNING("%s", session.id);
        if (session.id != NULL) {
            INFO("geckodriver is ready");
            ctx->session = session;
            cJSON_Delete(response);
            return 0;
        }

        DEBUG("waiting for geckodriver to be ready... (%d/%d)", retry + 1, max_retries);
        if (response) cJSON_Delete(response);
        sleep(1);
        retry++;
    }

    DEBUG("geckodriver failed to start after %d attempts");
    return -1;
}
