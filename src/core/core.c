#include <stdio.h>
#include "core.h"

#include "../communication_internal/communication.h"

void web_reset_context(web_context *ctx) {
    web_free_session(ctx->session);
    web_reset_last_error(ctx);
}

int web_init(web_context *ctx, char *geckodriverPath, char *firefoxPath, int port, int force_kill) {
    if (geckodriverPath != NULL) {
        ctx->geckodriverPath = geckodriverPath;
    } else {
        ctx->geckodriverPath = "geckodriver";
    }
    if (firefoxPath != NULL) {
        ctx->firefoxPath = firefoxPath;
    } else {
        ctx->firefoxPath = "firefox";
    }
    if (port != 0) {
        ctx->port = port;
    } else {
        ctx->port = 9515;
    }
    ctx->session = (web_session){NULL, NULL};
    ctx->last_error = (web_error){0, NULL, NULL, NULL};

    DEBUG("--------\ngeckodriverPath='%s', firefoxPath='%s', port=%d\n--------",
          ctx->geckodriverPath, ctx->firefoxPath,  ctx->port);

    int err = 0;
    if ((err = gecko_run(ctx, force_kill)) < 0) {
        DEBUG("Failed to start geckodriver");
        web_free_session(ctx->session);
        return err;
    }

    if ((err = wait_for_gecko_ready(ctx)) < 0) {
        DEBUG("Geckodriver failed to start");
        web_free_session(ctx->session);
        return err;
    }

    DEBUG("extracted session.id='%s'", ctx->session.id);

    return 0;
}

int web_close(web_context *ctx) {
    DEBUG("closing session '%s'", ctx->session.id);
    CURL *curl = curl_easy_init();

    char endpoint[256];
    sprintf(endpoint, "http://127.0.0.1:%d/session/%s", ctx->port, ctx->session.id);
    DEBUG("endpoint='%s'", endpoint);

    curl_easy_setopt(curl, CURLOPT_URL, endpoint);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    web_reset_context(ctx);

    return 0;
}

int web_usleep(int microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    return nanosleep(&ts, NULL);
}

web_timeouts web_get_timeouts(web_context *ctx) {
    cJSON *response_json = NULL;
    RCS(ctx, "/timeouts", NULL, &response_json, WEB_GET);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    web_timeouts timeouts;
    timeouts.script_ms = cJSON_GetObjectItemCaseSensitive(value, "script")->valueint;
    timeouts.page_load_ms = cJSON_GetObjectItemCaseSensitive(value, "pageLoad")->valueint;
    timeouts.implicit_wait_ms = cJSON_GetObjectItemCaseSensitive(value, "implicit")->valueint;
    cJSON_Delete(response_json);

    DEBUG("Got timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms", timeouts.script_ms, timeouts.page_load_ms,
          timeouts.implicit_wait_ms);
    return timeouts;
}

int web_set_timeouts(web_context *ctx, web_timeouts timeouts) {
    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(request_json, "script", timeouts.script_ms);
    cJSON_AddNumberToObject(request_json, "pageLoad", timeouts.page_load_ms);
    cJSON_AddNumberToObject(request_json, "implicit", timeouts.implicit_wait_ms);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    int status = RCS(ctx, "/timeouts", request_str, &response_json, WEB_POST);
    DEBUG_JSON(response_json);

    free(request_str);
    cJSON_Delete(response_json);

    DEBUG("Set timeouts to: script=%d ms, pageLoad=%d ms, implicit=%d ms", timeouts.script_ms, timeouts.page_load_ms,
          timeouts.implicit_wait_ms);
    return status;
}

web_status web_get_status(web_context *ctx) {
    cJSON *response_json = NULL;
    RCS(ctx, "/status", NULL, &response_json, WEB_GET);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    web_status status;
    status.ready = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(value, "ready"));
    status.message = strdup(cJSON_GetObjectItemCaseSensitive(value, "message")->valuestring);
    cJSON_Delete(response_json);

    DEBUG("Got status: ready='%s', message='%s'", status.ready, status.message);
    return status;
}

void free_status(web_status status) {
    if (status.message) {
        free(status.message);
    }
}

int web_create_session(web_context *ctx, web_session *session) {
    cJSON *response = NULL;

    int status = run_curl(ctx, "/session",
          "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
          &response, WEB_POST);

    DEBUG("Create session status: %d", status);

    if (status < 0) {
        session->id = NULL;
        session->capabilities = NULL;
        return status;
    }

    DEBUG_JSON(response);

    cJSON *val = cJSON_GetObjectItem(response, "value");
    DEBUG_JSON(val);

    session->id = strdup(cJSON_GetObjectItem(val, "sessionId")->valuestring);
    DEBUG("session id='%s'", session->id);
    session->capabilities = cJSON_Duplicate(cJSON_GetObjectItem(val, "capabilities"), true);
    DEBUG("capabilities='%s'", cJSON_Print(session->capabilities));
    cJSON_Delete(response);

    DEBUG("Created session with id='%s'", session->id);

    return status;
}

void web_free_session(web_session session) {
    if (session.id) {
        free(session.id);
        session.id = NULL;
    }
    if (session.capabilities) {
        cJSON_Delete(session.capabilities);
        session.capabilities = NULL;
    }
}

void web_reset_last_error(web_context *ctx) {
    if (ctx->last_error.path) {
        free(ctx->last_error.path);
        ctx->last_error.path = NULL;
    }
    if (ctx->last_error.error) {
        free(ctx->last_error.error);
        ctx->last_error.error = NULL;
    }
    if (ctx->last_error.message) {
        free(ctx->last_error.message);
        ctx->last_error.message = NULL;
    }
    ctx->last_error.code = 0;
}

web_error web_get_last_error(web_context *ctx) {
    return ctx->last_error;
}

void web_set_last_error(web_context *ctx, web_error error) {
    web_reset_last_error(ctx);
    ctx->last_error = error;
}