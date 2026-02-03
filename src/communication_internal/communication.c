#include "communication.h"

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    web_resp_buf *rb = (web_resp_buf *) userdata;

    size_t needed = rb->len + total + 1;
    if (needed > rb->cap) {
        size_t new_cap = rb->cap ? rb->cap * 2 : 4096;
        while (new_cap < needed) new_cap *= 2;
        char *new_mem = realloc(rb->buf, new_cap);
        if (!new_mem) return 0; // Trigger CURLE_WRITE_ERROR
        rb->buf = new_mem;
        rb->cap = new_cap;
    }

    memcpy(rb->buf + rb->len, ptr, total);
    rb->len += total;
    rb->buf[rb->len] = '\0';

    DEBUG("received %zu bytes, len=%zu", total, rb->len);
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
    web_resp_buf rb = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rb);

    long http_code = 0;
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // If buf is NULL (no content), treat as empty string
    char *body = rb.buf ? rb.buf : "";

    if (res != CURLE_OK) {
        DEBUG("curl_easy_perform failed: %s", curl_easy_strerror(res));
        web_error err = {0};
        err.code = -1;
        err.path = strdup(url);
        err.error = strdup("CURL request failed");
        err.message = strdup(curl_easy_strerror(res));
        web_set_last_error(ctx, err);
        curl_easy_cleanup(curl);
        if (rb.buf) free(rb.buf);
        return -1;
    } else {
        DEBUG("request OK, HTTP response code: %ld", http_code);
        DEBUG("response: %s", body);

        if (http_code < 200 || http_code >= 300) {
            DEBUG("HTTP error code %ld received", http_code);
            http_code = -http_code;
            response_json = NULL;
            cJSON *resp = cJSON_Parse(body);
            curl_easy_cleanup(curl);

            if (!resp) {
                web_error err = {0};
                err.code = http_code;
                err.path = strdup(url);
                if (http_code == -405) {
                    DEBUG("Method Not Allowed (405) error without JSON body");
                    err.error = strdup("Method not allowed");
                    err.message = strdup("The requested HTTP method is not allowed for the given URL.");
                } else {
                    DEBUG("Failed to parse error response JSON");
                    err.error = strdup("Unexpected error");
                    err.message = strdup(body);
                }
                web_set_last_error(ctx, err);
                if (rb.buf) free(rb.buf);
                return http_code;
            }

            cJSON *error_info = cJSON_GetObjectItemCaseSensitive(resp, "value");
            web_error err = {0};
            err.code = http_code;
            err.path = strdup(url);
            err.error = strdup(cJSON_GetObjectItemCaseSensitive(error_info, "error")->valuestring);
            err.message = strdup(cJSON_GetObjectItemCaseSensitive(error_info, "message")->valuestring);
            web_set_last_error(ctx, err);
            cJSON_Delete(resp);
            if (rb.buf) free(rb.buf);
            return http_code;
        }

        if (response_json) {
            *response_json = cJSON_Parse(body);
        }
        web_reset_last_error(ctx);
    }
    curl_easy_cleanup(curl);
    if (rb.buf) free(rb.buf);
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

int validate_executable(const char *path, char *error_msg, size_t error_len) {
    if (path == NULL || strlen(path) == 0) {
        snprintf(error_msg, error_len, "Path is empty");
        return -1;
    }

    // F_OK = exists, X_OK = is executable
    if (access(path, F_OK) != 0) {
        snprintf(error_msg, error_len, "File not found");
        return -2;
    }
    if (access(path, X_OK) != 0) {
        snprintf(error_msg, error_len, "File not executable (check permissions)");
        return -3;
    }

    return 0;
}

int gecko_run(web_context *ctx, int force_kill, int ignore_gecko_checks, int ignore_firefox_checks) {
    char out_error[1024] = {0};
    size_t error_size = sizeof(out_error);

    if (force_kill) {
        char kill_cmd[256];
        snprintf(kill_cmd, sizeof(kill_cmd), "fuser -k -n tcp %d > /dev/null 2>&1", ctx->port);
        system(kill_cmd);
        DEBUG("Cleaned up port %d", ctx->port);
        sleep(1);
    }

    // pre-checks
    int validate = 0;
    if (ignore_gecko_checks == 0 && (validate = validate_executable(ctx->geckodriverPath, out_error, error_size)) < 0) {
        DEBUG("Geckodriver validation failed: %s", out_error);
        web_error err = {
            .code = validate,
            .path = strdup(ctx->geckodriverPath),
            .error = strdup("Geckodriver validation failed"),
            .message = strdup(out_error)
        };
        web_set_last_error(ctx, err);
        return validate;
    }
    DEBUG("Geckodriver binary found: %s", ctx->geckodriverPath);
    if (ignore_firefox_checks == 0 && access(ctx->firefoxPath, F_OK | R_OK) != 0) {
        DEBUG("Firefox binary validation failed");
        snprintf(out_error, error_size, "Firefox binary not found or not readable");
        web_error err = {
            .code = -2,
            .path = strdup(ctx->firefoxPath),
            .error = strdup("Firefox validation failed"),
            .message = strdup(out_error)
        };
        web_set_last_error(ctx, err);
        return -4;
    }
    DEBUG("Firefox binary found: %s", ctx->firefoxPath);

    char cmd[4096];
    char *log_file = "/tmp/geckodriver_output.log";

    snprintf(cmd, sizeof(cmd),
             "%s --port %d --binary \"%s\" > %s 2>&1 &",
             ctx->geckodriverPath,
             ctx->port,
             ctx->firefoxPath,
             log_file);

    DEBUG("Executing: %s", cmd);

    int res = system(cmd);

    if (res != 0) {
        snprintf(out_error, error_size, "Failed to launch shell command (code %d). Check %s for details.", res,
                 log_file);
        web_error err = {
            .code = -4,
            .path = strdup(cmd),
            .error = strdup("Failed to launch shell command"),
            .message = strdup(out_error)
        };
        web_set_last_error(ctx, err);
        return -4;
    }

    web_usleep(500000);

    FILE *fp = fopen(log_file, "r");
    if (fp) {
        char line[512];
        // Read the log file line by line
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "Address already in use") ||
                strstr(line, "unknown option") ||
                strstr(line, "operation not permitted") ||
                strstr(line, "Error")) {
                snprintf(out_error, error_size, "Startup failed: %s", line);

                size_t len = strlen(out_error);
                if (len > 0 && out_error[len - 1] == '\n') {
                    out_error[len - 1] = '\0';
                }
                fclose(fp);

                web_error err = {
                    .code = -5,
                    .path = strdup(line),
                    .error = strdup("Geckodriver startup error"),
                    .message = strdup(out_error)
                };
                web_set_last_error(ctx, err);
                return -5;
            }
        }
        fclose(fp);
    } else {
        snprintf(out_error, error_size, "Could not read startup log at %s", log_file);
        web_error err = {
            .code = -6,
            .path = log_file,
            .error = strdup("Could not read geckodriver log"),
            .message = strdup(out_error)
        };
        web_set_last_error(ctx, err);
        return -6;
    }

    return 0; // Success
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
            web_usleep(500000); // 0.5 second
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
        web_usleep(500000); // 0.5 second
        retry++;
    }

    DEBUG("geckodriver failed to start after %d attempts");
    web_error err = {
        .code = -5,
        .path = NULL,
        .error = strdup("geckodriver failed to start"),
        .message = strdup("geckodriver failed to start within the expected time")
    };
    web_set_last_error(ctx, err);
    return -5;
}
