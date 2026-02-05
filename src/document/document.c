#include <stdio.h>
#include "document.h"

#include "communication_internal/communication.h"

int web_get_page_source(web_context *ctx, char **source) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, source);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/source", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    DEBUG("%d", resp);
    if (resp < 0) {
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    *source = strdup(value->valuestring);

    cJSON_Delete(response_json);
    return resp;
}

int web_execute_script(web_context *ctx, const char *script, cJSON *args, cJSON **result, int async) {
    CHECK_NULL(ctx, script);

    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddStringToObject(request_json, "script", script);
    if (args) {
        cJSON_AddItemToObject(request_json, "args", cJSON_Duplicate(args, 1));
    } else {
        cJSON_AddItemToObject(request_json, "args", cJSON_CreateArray());
    }

    char *request_str = cJSON_PrintUnformatted(request_json);
    DEBUG("Request JSON: %s", request_str);

    cJSON *response_json = NULL;
    int resp = 0;
    if (async) {
        resp = RCS(ctx, "/execute/async", request_str, &response_json, WEB_POST);
    } else {
        resp = RCS(ctx, "/execute/sync", request_str, &response_json, WEB_POST);
    }

    free(request_str);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    if (result) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        *result = cJSON_Duplicate(value, 1);
    }

    cJSON_Delete(request_json);
    cJSON_Delete(response_json);
    return resp;
}

int web_execute_script_sync(web_context *ctx, const char *script, cJSON *args, cJSON **result) {
    CHECK_NULL(ctx, script);
    return web_execute_script(ctx, script, args, result, 0);
}

int web_execute_script_async(web_context *ctx, const char *script, cJSON *args, cJSON **result) {
    CHECK_NULL(ctx, script);
    return web_execute_script(ctx, script, args, result, 1);
}
