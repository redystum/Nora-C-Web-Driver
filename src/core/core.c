#include <stdio.h>
#include "core.h"

#include "../communication/communication.h"

/**
 * \brief Reset web context mantaining paths and port
 * \param ctx  web context
 */
void web_reset_context(web_context *ctx) {
    web_free_session(ctx->session);
}

/**
 * \brief Initialize web context
 * \param geckodriverPath NULL = "geckodriver"
 * \param firefoxPath NULL = "firefox"
 * \param port 0 = 9515
 * \param force_kill 1 = kill existing geckodriver on port
 * \return web context
 */
web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill) {
    web_context ctx = {0};
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

    if (_gecko_run(ctx, force_kill) < 0) {
        DEBUG("Failed to start geckodriver");
        web_reset_context(&ctx);
        return ctx;
    }

    if (_wait_for_gecko_ready(&ctx) < 0) {
        DEBUG("Geckodriver failed to start");
        web_reset_context(&ctx);
        return ctx;
    }

    DEBUG("extracted session.id='%s'", ctx.session.id);

    return ctx;
}


/**
 * \brief Close web session
 * \param ctx  web context
 * \return 0=ok
 */
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

web_timeouts web_get_timeouts(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/timeouts", NULL, &response_json, GET);

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

int web_set_timeouts(web_context ctx, web_timeouts timeouts) {
    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(request_json, "script", timeouts.script_ms);
    cJSON_AddNumberToObject(request_json, "pageLoad", timeouts.page_load_ms);
    cJSON_AddNumberToObject(request_json, "implicit", timeouts.implicit_wait_ms);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    _rcs(ctx, "/timeouts", request_str, &response_json, POST);
    DEBUG_JSON(response_json);

    free(request_str);
    cJSON_Delete(response_json);

    DEBUG("Set timeouts to: script=%d ms, pageLoad=%d ms, implicit=%d ms", timeouts.script_ms, timeouts.page_load_ms,
          timeouts.implicit_wait_ms);
    return 0;
}

web_status web_get_status(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/status", NULL, &response_json, GET);

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

web_session web_create_session(web_context ctx) {
    web_session session;
    cJSON *response = NULL;

    int status = _run_curl(ctx, "/session",
          "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
          &response, POST);

    DEBUG("Create session status: %d", status);

    if (status < 0) {
        session.id = NULL;
        session.capabilities = NULL;
        return session;
    }

    cJSON *val = cJSON_GetObjectItem(response, "value");
    DEBUG_JSON(val);

    session.id = strdup(cJSON_GetObjectItem(val, "sessionId")->valuestring);
    DEBUG("session id='%s'", session.id);
    session.capabilities = cJSON_Duplicate(cJSON_GetObjectItem(val, "capabilities"), true);
    DEBUG("capabilities='%s'", cJSON_Print(session.capabilities));
    cJSON_Delete(response);

    DEBUG("Created session with id='%s'", session.id);

    return session;
}

void web_free_session(web_session session) {
    if (session.id) {
        free(session.id);
    }
    if (session.capabilities) {
        cJSON_Delete(session.capabilities);
    }
}