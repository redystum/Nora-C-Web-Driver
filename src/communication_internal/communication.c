#include "communication.h"

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    web_resp_buf *rb = (web_resp_buf *) userdata;
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

int run_curl(web_context *ctx, char *path, char *data, cJSON **response_json, web_request_method method) {
    DEBUG("path='%s' data='%s'", path ? path : "(null)",
          data ? data : "(null)");
    CURL *curl = curl_easy_init();
    if (!curl) {
        DEBUG("curl_easy_init failed");
        return -1;
    }

    char url[1024] = {0};
    if (path && path[0] == '/') {
        DEBUG("path starts with /");
        sprintf(url, "http://127.0.0.1:%d%s", ctx->port, path);
    } else if (path) {
        DEBUG("path does not start with /");
        sprintf(url, "http://127.0.0.1:%d/%s", ctx->port, path);
    }
    DEBUG("url='%s'", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    if (method == WEB_POST) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (data != NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        } else {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
        }
    } else if (method == WEB_DELETE) {
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
    web_resp_buf rb;
    rb.buf = raw_buf;
    rb.cap = RESPONSE_CAP;
    rb.len = 0;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rb);

    long http_code = 0;
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (res != CURLE_OK) {
        DEBUG("curl_easy_perform failed: %s", curl_easy_strerror(res));
        return -1;
    } else {
        DEBUG("request OK, HTTP response code: %ld", http_code);
        DEBUG("response (truncated): %.200s", raw_buf);

        if (http_code < 200 || http_code >= 300) {
            DEBUG("HTTP error code %ld received", http_code);
            http_code = -http_code;
            response_json = NULL;
            cJSON *resp = cJSON_Parse(raw_buf);
            curl_easy_cleanup(curl);
            cJSON *error_info = cJSON_GetObjectItemCaseSensitive(resp, "value");
            web_error err = {0};
            err.code = http_code;
            err.url = strdup(url);
            err.error = strdup(cJSON_GetObjectItemCaseSensitive(error_info, "error")->valuestring);
            err.message = strdup(cJSON_GetObjectItemCaseSensitive(error_info, "message")->valuestring);
            ctx->last_error = err;
            cJSON_Delete(resp);
            return http_code;
        }

        if (response_json) {
            *response_json = cJSON_Parse(raw_buf);
        }
        web_reset_last_error(ctx);
    }
    curl_easy_cleanup(curl);
    return http_code;
}

int run_curl_session(web_context *ctx, char *path, char *data, cJSON **response, web_request_method method) {
    char url[2048];
    char p[1024];
    if (path && path[0] != '/') {
        sprintf(p, "/%s", path);
        path = p;
    }

    sprintf(url, "/session/%s%s", ctx->session.id, path ? path : "");
    DEBUG("session_url='%s' data='%s'", url, data);
    return run_curl(ctx, url, data, response, method);
}

int gecko_run(web_context *ctx, int force_kill) {
    if (force_kill) {
        char kill_cmd[256] = {0};
        sprintf(kill_cmd, "fuser -k -n tcp %d > /dev/null 2>&1", ctx->port);
        int res = system(kill_cmd);
        DEBUG("killed existing geckodriver on port %d, res=%d",
              ctx->port, res);
        sleep(1);
    }

    char cmd[2048] = {0};
    char command[1024] = {0};
    sprintf(command, "--port %d --binary %s > /dev/null 2>&1 &", ctx->port, ctx->firefoxPath);
    DEBUG("starting geckodriver with command fragment: %s", command);

    sprintf(cmd, "%s %s", ctx->geckodriverPath, command);
    DEBUG("executing command: %s", cmd);
    int res = system(cmd);
    DEBUG("geckodriver start command returned %d", res);
    return res;
}

int wait_for_gecko_ready(web_context *ctx) {
    cJSON *response = NULL;
    int max_retries = 30;
    int retry = 0;

    while (retry < max_retries) {
        web_session session;
        int status = web_create_session(ctx, &session);
        if (status < 0) {
            DEBUG("web_create_session failed with status %d", status);
            sleep(1);
            retry++;
            continue;
        }
        DEBUG("session.id='%s'", session.id ? session.id : "(null)");
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
